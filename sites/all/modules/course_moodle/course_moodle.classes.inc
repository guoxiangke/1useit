<?php

/**
 * @file course_moodle.classes.inc
 * CourseObject classes for Moodle to handle creation and tracking.
 * @todo convert to API calls when we can create Moodle objects remotely.
 */

/**
 * Moodle object base class.
 *
 * Provides sessioning support for remote course object creation.
 */
class CourseObjectMoodle extends CourseObject {

  private $cid = NULL;

  /**
   * Specify that this object needs a navigation listener.
   */
  public function hasPolling() {
    return TRUE;
  }

  function getHttpRequestAsMoodleAdmin($url) {
    $session = $this->getSession();
    $headers['Cookie'] = $session['cookie'];
    $response = drupal_http_request($url, array('headers' => array('headers' => array('headers' => $headers))));
    return $response;
  }

  function getContentFromUrl($url) {
    global $base_url;
    print($base_url . $url);
    return $this->getHttpRequestAsMoodleAdmin($base_url . $url)->data;
  }

  /**
   * Return a table from a CSV.
   */
  function csv2table($csv_data) {
    $out = '';
    $temp = fopen("php://memory", "rw");
    fwrite($temp, $csv_data);
    fseek($temp, 0);
    while ($line = fgetcsv($temp, 0, "\t")) {
      $row = array();

      foreach ($line as $cell) {
        $row[] = $cell;
      }

      if (!$headers) {
        $headers = $row;
      }
      else {
        $rows[] = $row;
      }
    }
    fclose($temp);
    $out .= theme_table($headers, $rows);
    return $out;
  }

  /**
   * Get the primary report URL for this moodle object.
   *
   * @return array
   *   Array of
   *     'title': The title of the object
   *     'url':  URL of the object's report
   */
  function getReportInfo() {
    $snid = $this->getId();
    $sql = "SELECT cr.*, mcm.instance as m_instance
    FROM {mdl_course_modules} mcm, {course_outline} cr, {course_moodle} cn
    WHERE cr.nid = cn.nid
    AND mcm.course  = cn.cid
    AND cr.instance = mcm.id
    AND cr.snid = :snid";
    $result = db_query($sql, array(':snid' => $snid));

    // Disable PURL in external URLs.
    $purl_disable = array(
      'purl' => array(
        'disabled' => TRUE,
      ),
    );

    $row = $result->fetch();
    switch ($row->requirement_component) {
      case 'quiz':
        $url = 'moodle/mod/quiz/report.php';
        $link = url($url, array('query' => array('q' => $row->m_instance)) + $purl_disable);
        break;
      case 'questionnaire':
        $url = 'moodle/mod/questionnaire/report.php';
        $link = url($url, array('query' => array('instance' => $row->m_instance, 'sid' => $row->m_instance, 'action' => 'vall')) + $purl_disable);
        break;
      case 'scorm':
        $url = 'moodle/mod/scorm/report.php';
        $link = url($url, array('query' => array('id' => $row->instance)) + $purl_disable);
        break;
    }

    if ($link) {
      return array(
        'title' => $row->title,
        'url' => $link,
      );
    }
  }

  /**
   * Return the moodle component instance (different than the course instance).
   */
  function getMoodleInstanceId() {
    $snid = $this->getId();

    $sql = "SELECT cr.*, mcm.instance as m_instance
      FROM {mdl_course_modules} mcm, {course_outline} cr, {course_node} cn
      WHERE cr.nid = cn.nid
      AND mcm.course  = cn.cid
      AND cr.instance = mcm.id
      AND cr.snid = :snid";
    $result = db_query($sql, array(':snid' => $snid));
    $row = $result->fetch();
    return $row->m_instance;
  }

  /**
   * $csv is an array of rows, each with an 'idnumber' element being the drupal user ID.
   * @param type $csv
   * @return type
   */
  function attachProfile($csv) {
    foreach ($csv as $row) {
      $users[] = $row['idnumber'];
    }

    $view = views_get_view('course_moodle_profile');
    $view->set_arguments(
      array(
        implode(',', array_filter(array_values($users))),
      )
    );
    $view->render();

    foreach ($view->style_plugin->rendered_fields as $rowidx => $values) {
      $uid = $values['uid'];
      foreach ($values as $fieldname => $value) {
        $viewarray[$uid][$view->field[$fieldname]->options['label']] = $value;
      }
    }

    foreach ($csv as $rid => &$response) {
      if ($viewarray[$response['idnumber']]) {
        $response = array_merge($response, $viewarray[$response['idnumber']]);
      }
    }

    return $csv;
  }

  function renderCSV($dcsv, $headers = NULL) {
    $fh = fopen('php://temp/maxmemory:' . (12 * 1024 * 1024), 'r+');
    if ($headers) {
      fputcsv($fh, $headers);
    }
    foreach ($dcsv as $row) {
      if (!$headers) {
        fputcsv($fh, array_keys($row));
        $headers = TRUE;
      }
      fputcsv($fh, $row);
    }
    rewind($fh);
    $output = stream_get_contents($fh);
    fclose($fh);
    header('Content-type: text/csv');
    header('Content-disposition: attachment; filename=' . $this->getCourseNid() . '-' . str_replace(' ', '-', $this->getTitle()) . '.csv');
    print $output;
    exit();
  }

  /**
   * Create a Moodle course to associate course objects to.
   */
  function create() {
    $courseNode = node_load($this->getCourseNid());
    if (!$courseNode->course_moodle['cid']) {
      $courseNode->course_moodle['cid'] = $this->createCourse($courseNode);
      course_moodle_node_update($courseNode, 'update', NULL, NULL);
    }
  }

  /**
   * Create a Moodle course.
   *
   * @return int
   *   Moodle course ID.
   */
  public static function createCourse($courseNode) {
    $params = array(
      'courses' => array(
        'fullname' => $courseNode->title,
        'categoryid' => 1,
        'shortname' => $courseNode->course_moodle['machine_name'],
        'idnumber' => $courseNode->course['external_id'],
      ),
    );
    $result = course_moodle_call('core_course_create_courses', $params);
    return $result[0]['id'];
  }

  /**
   * Verify shared session and get sesskey:
   */
  static function getSession() {
    global $base_url;
    static $session_info = array();

    if (!empty($session_info)) {
      return $session_info;
    }

    $name = session_name();
    $value = session_id();
    $cookie = "$name=$value;";

    // url of the moodle-drupal homepage
    $url = course_moodle_path(TRUE) . '/login/index.php';

    // necessary for the POST method
    $headers = array('Cookie' => $cookie);
    // if successful, parse to get the sesskey
    $login_response = drupal_http_request($url, array('headers' => array('headers' => array('headers' => $headers))));
    preg_match_all('/MoodleSession=(.*);/U', $login_response->headers['Set-Cookie'], $matches);
    foreach ($matches as $match) {
      $last_session = $match[1];
    }
    $headers['Cookie'] .= "MoodleSession=$last_session";
    $response = drupal_http_request(course_moodle_path(TRUE), array('headers' => array('headers' => array('headers' => $headers))));
    preg_match('/sesskey":"(\w+)/', $response->data, $match);
    if (isset($match[1])) {
      $sesskey = $match[1];
    }

    if (isset($sesskey) && strlen($sesskey)) {
      $session_info = array(
        'cookie' => $headers['Cookie'],
        'sesskey' => $sesskey,
      );
      return $session_info;
    }
    else {
      return NULL;
    }
  }

  public function getTakeType() {
    return 'iframe';
  }

  public function take() {
    // Save an empty fulfillment record upon starting.
    if (!$this->getFulfillment()->getId()) {
      $this->getFulfillment()->save();
    }

    // Set a cookie with a URL to the course (for externservercourse.php)
    // @todo probably need to set up something for when moodle is run off-site,
    // and on a different domain. services maybe? a service to retrieve the
    // course path in drupal from a moodle course id
    setcookie('course_moodle_course_url', url("course_moodle/escapeframe/{$this->getCourseNid()}", array('absolute' => TRUE)), 0, '/');
  }

  public function getEditUrl() {
    return url("node/{$this->getCourseNid()}/course-outline/course-moodle/{$this->getId()}/edit");
  }

  function getMoodleEditUrl() {
    if (!$this->getInstanceId()) {
      // Create new course object.
      return course_moodle_path(TRUE) . "/course/modedit.php?section=0&add={$this->getComponent()}&course={$this->getCourse()->getNode()->course_moodle['cid']}";
    }
    else {
      // Edit existing course object.
      return course_moodle_path(TRUE) . "/course/modedit.php?update={$this->getInstanceId()}";
    }
  }

  function getMoodleQuestionsEditUrl() {

  }

}

class CourseObjectMoodleResource extends CourseObjectMoodle {

  function poll() {
    global $user;
    global $_course_moodle_prefix;
    $uid = $user->uid;

    $resource = $this->getComponent();
    $cid = db_query('select cid from {course_moodle} where nid = :nid', array(':nid' => $this->getCourseNid()))->fetchField();

    if (!$moodle_course_module = course_moodle_get_coursemodule_from_id('resource', $requirement['instance'])) {
      //error('Course Module ID was incorrect');
    }
    if (!$resource = course_moodle_moodle_get_record('resource', 'id', $moodle_course_module->instance)) {
      //error('Resource ID was incorrect');
    }

    /**/
    //currently supported Moodle resources:
    // note: 'resource' changed to 'page'
    $sql_check_views = "SELECT ml.*, mu.idnumber as uid FROM {$_course_moodle_prefix}log ml, {$_course_moodle_prefix}user mu WHERE ml.module ='page' AND ml.userid=mu.id AND action='view' AND  ml.course= :cid AND cmid = :instance AND  mu.idnumber = :uid";
    $result_check_view = db_query($sql_check_views, array(':cid' => $cid, ':instance' => $this->getInstanceId(), ':uid' => $uid));
    while ($row = $result_check_view->fetch()) {
      $array_check_view = $row;
    }

    if (is_array($array_check_view) && $array_check_view['id']) {
      $this->getFulfillment()->setComplete(1)->save();
    }
  }

  public function getTakeUrl() {
    return url('moodle/mod/resource/view.php', array('purl' => array('disabled' => TRUE), 'query' => array('id' => $this->getInstanceId())));
  }

}

class CourseObjectMoodleQuiz extends CourseObjectMoodle {

  function getOptionsSummary() {
    $summary = parent::getOptionsSummary();
    if ($this->getInstanceId()) {
      $summary['questions'] = l('Edit questions', "node/{$this->getCourseNid()}/course-outline/course-moodle/{$this->getId()}/questions");
    }
    return $summary;
  }

  /**
   *
   * @param type $instance
   * @return type
   */
  function getOverviewCSV() {
    $instance = $this->getMoodleInstanceId();
    // Get array of Drupal user IDs from Moodle CSV
    $users = array();
    $csv = array();

    $url = url("moodle/mod/quiz/report.php", array(
      'query' => "q=$instance&mode=responses&attemptsmode=0&qmfilter=1&download=CSV",
      'absolute' => TRUE,
      ));
    $response = $this->getHttpRequestAsMoodleAdmin($url);
    $text = explode("\n", trim($response->data));
    foreach ($text as $line) {
      $fields = explode("\t", $line);

      unset($fields[0], $fields[1], $fields[3], $fields[4]);
      if (!$skip) {
        $headers = $fields;
        $fields[] = 'Title';
        $skip = TRUE;
      }
      else {
        $fields = array_pad($fields, count($headers), '');
        $newline = array_combine($headers, $fields);
        $newline['title'] = $this->getTitle();
        $newline['idnumber'] = $newline['ID number'];
        $csv[] = $newline;
      }
    }
    return $csv;
  }

  function getReports() {
    return array(
      'overview' => array(
        'title' => 'Overview',
        'default' => TRUE,
        'children' => array(
          'csv' => array(
            'file' => TRUE,
            'title' => 'Export with profile data',
          ),
        ),
      ),
      'compare' => array(
        'title' => 'Analysis/Compare',
      ),
    );
  }

  function getReport($key, $subkey = NULL) {
    if (!$subkey) {
      $reports = $this->getReports();
      switch ($key) {
        case 'overview':
          $report = $this->getReportInfo();
          return array(
            'title' => $reports[$key]['title'],
            'url' => $report['url'],
          );
        case 'item_analysis':
          $instance = $this->getMoodleInstanceId();
          $url = url("/moodle/mod/quiz/report.php?q=$instance&mode=analysis", array('external' => TRUE));
          return array(
            'title' => $reports[$key]['title'],
            'url' => $url,
          );
        case 'compare':
          return array(
            'title' => 'Comparison',
            'content' => $this->renderCompare(),
          );
      }
    }
    elseif ($subkey == 'csv') {
      return array(
        'content' => $this->renderOverview(),
      );
    }
  }

  /**
   * Render comparison report.
   * @return type
   */
  function renderCompare() {
    $op = $_GET['op'];
    $snid = $_GET['snid'];

    $out = '';

    $compare = $snid && in_array($op, array('Compare', 'Download comparison'));
    $compare_csv = ($op == 'Download comparison');

    $out .= drupal_get_form('course_moodle_quiz_comparison_form', $this);


    $instance = $this->getInstanceId();

    $url = url('moodle/mod/quiz/report.php', array('query' => array('mode' => 'analysis', 'noheader' => 'yes', 'download' => 'CSV', 'id' => ''), 'absolute' => TRUE));
    $response_base = $this->getHttpRequestAsMoodleAdmin($url . $instance);
    $questions_base = $this->analysisToArray($response_base->data, $headers);

    $strip_columns = array('Question text');

    if ($compare) {
      // Strip out columns that wouldn't make sense.
      $strip_columns = array_merge($strip_columns, array('SD', 'Disc. Index', 'Disc. Coeff. '));
      $compare_additional_headers = array('R. Counts', 'R.%', 'Q. count', '% Correct Facility');
      $headers = array_merge($headers, $compare_additional_headers);
    }

    if ($compare) {
      // If comparing, load the other object. Check to see if it has the same questions.
      $comparedCourseObject = new CourseObjectMoodleQuiz($snid);
      $response_compare = $this->getHttpRequestAsMoodleAdmin($url . $comparedCourseObject->getInstanceId());
      $questions_compare = $this->analysisToArray($response_compare->data);
      if (array_filter(array_diff_key($questions_base, $questions_compare))) {
        return $out . "Sorry, you must compare two tests with identical questions.";
      }
    }

    // Strip headers we don't need.
    foreach ($headers as $key => $header) {
      if (in_array($header, $strip_columns)) {
        unset($headers[$key]);
      }
    }

    // The position where the header starts to show which results are Pre/Post.
    $header_compare_start = 5;

    if ($compare_csv) {
      $rows[0] = array_pad(array(), count($headers), '');
    }
    else {
      $rows[0][0] = array(
        'data' => '',
        'colspan' => $header_compare_start,
      );
    }

    if ($compare) {
      $header_break = $header_compare_start + count($compare_additional_headers);

      $rows[0][$header_compare_start] = $this->getTitle();
      $rows[0][$header_compare_start + count($compare_additional_headers)] = $comparedCourseObject->getTitle();

      if ($_REQUEST['op'] != 'CSV') {
        $rows[0][$header_compare_start] = array(
          'data' => $rows[0][$header_compare_start],
          'colspan' => count($compare_additional_headers),
          'class' => 'test-1',
        );

        $rows[0][$header_break] = array(
          'data' => $rows[0][$header_break],
          'colspan' => count($compare_additional_headers),
          'class' => 'test-2',
        );
      }
    }

    // Combine reports to compare.
    foreach ($questions_base as $qno => $question) {
      foreach ($question as $rid => $response) {
        $row = array();
        $cell = 0;
        foreach ($response as $key => $field) {
          if (!in_array($key, $strip_columns)) {
            $cell++;
            if ($compare && !$compare_csv && $cell >= $header_compare_start + 1) {
              $row[] = array(
                'data' => $field,
                'class' => 'quiz-1',
              );
            }
            else {
              $row[] = $field;
            }
          }
        }

        if ($compare) {
          foreach ($questions_compare[$qno][$rid] as $key => $field) {
            if (in_array($key, array('R. Counts', 'R.%', 'Q. count', '% Correct Facility'))) {
              if (!$compare_csv) {
                $row[] = array(
                  'data' => $field,
                  'class' => 'quiz-2',
                );
              }
              else {
                $row[] = $field;
              }
            }
          }
        }

        if (!$compare_csv) {
          $rows[] = array(
            'data' => $row,
            'class' => $row[0] ? 'question' : '',
          );
        }
        else {
          $rows[] = $row;
        }
      }
    }

    if ($compare_csv) {
      $this->renderCSV($rows, $headers);
    }
    else {
      $out .= theme_table($headers, $rows);
    }

    if ($op == 'Download responses') {
      $mcsv = $this->getOverviewCSV();
      $courseObjectCompare = new CourseObjectMoodleQuiz($snid);
      $ccsv = $courseObjectCompare->getOverviewCSV();
      $combined = array_merge($mcsv, $ccsv);
      $this->renderCSV($combined);
    }

    return $out;
  }

  function analysisToArray($data, &$headers = NULL) {
    $csv = explode("\n", trim($data));
    $questions = array();
    foreach ($csv as $line) {
      $line = explode("\t", $line);
      if (!$headers) {
        $headers = $line;
      }

      $line = array_pad($line, count($headers), '');

      if (is_numeric($line[0]) && $line[0] != $inquestion) {
        // This is the start of a question, or continuation.
        $inquestion = $line[0];
      }

      if ($inquestion) {
        $questions[$inquestion][] = array_combine($headers, $line);
      }
    }

    return $questions;
  }

  function renderOverview() {
    $mcsv = $this->getOverviewCSV();
    $dcsv = $this->attachProfile($mcsv);
    $this->renderCSV($dcsv);
  }

  function poll() {
    global $user;
    global $_course_moodle_prefix;

    $requirement = $this->getOptions();
    //'Quiz'
    if ($requirement['instance']) {
      if (!$moodle_course_module = course_moodle_get_coursemodule_from_id('quiz', $requirement['instance'])) {
        //error("There is no coursemodule with id $id");
      }
      if (!$moodle_quiz = course_moodle_moodle_get_record("quiz", "id", $moodle_course_module->instance)) {
        //error("The quiz with id $cm->instance corresponding to this coursemodule $id is missing");
      }
    }
    $sql_get_quizzes = "SELECT mqa.* from {$_course_moodle_prefix}quiz_attempts mqa
      INNER JOIN {$_course_moodle_prefix}user u on (u.id = mqa.userid)
      WHERE u.idnumber = :uid and mqa.quiz = :quizid
      ORDER BY mqa.sumgrades ASC";
    $results = db_query($sql_get_quizzes, array(':uid' => $user->uid, ':quizid' => $moodle_quiz->id));

    while ($row = $results->fetch(PDO::FETCH_ASSOC)) {
      $quiz_record = $row;
    }

    if ($requirement['required'] && (!is_array($quiz_record) || !$quiz_record['timefinish'])) {
      // If the quiz is required and they haven't finished an attempt, fail.
      return FALSE;
    }

    if (is_array($quiz_record)) {
      // @todo sumgrades is the number of points now - we need percentage for
      // Course right now
      if ($requirement['passing_grade'] <= intval($quiz_record['sumgrades']) && $quiz_record['timefinish'] > 0) {
        $this->getFulfillment()->setOption('grade_result', $quiz_record['sumgrades'])->setComplete(1)->save();
      }
    }
  }

  function getTakeUrl() {

    return url('moodle/mod/quiz/view.php', array('purl' => array('disabled' => TRUE), 'query' => array('id' => $this->getInstanceId())));
  }

  function getMoodleQuestionsEditUrl() {
    return url('moodle/mod/quiz/edit.php', array('purl' => array('disabled' => TRUE), 'query' => array('cmid' => $this->getInstanceId())));
  }

  /**
   * Moodle quiz options.
   */
  public function optionsDefinition() {
    $options = parent::optionsDefinition();

    $options['passing_grade'] = 75;

    return $options;
  }

  /**
   * Add a passing grade option.
   */
  public function optionsForm(&$form, &$form_state) {
    parent::optionsForm($form, $form_state);
    $defaults = $this->getOptions();

    $form['grading']['passing_grade'] = array(
      '#title' => t('Passing grade'),
      '#type' => 'textfield',
      '#size' => 4,
      '#default_value' => $defaults['passing_grade'],
      '#description' => t('The user will not be able to proceed past this object unless this grade is met.'),
    );
  }

  function isGraded() {
    return TRUE;
  }

}

class CourseObjectMoodleCeInfo extends CourseObjectMoodleResource {

}

class CourseObjectMoodleHtmlCourse extends CourseObjectMoodleResource {

}

class CourseObjectMoodleFile extends CourseObjectMoodleResource {

}

class CourseObjectMoodleQuestionnaire extends CourseObjectMoodle {

  function getOptionsSummary() {
    $summary = parent::getOptionsSummary();
    if ($this->getInstanceId()) {
      $summary['questions'] = l('Edit questions', "node/{$this->getCourseNid()}/course-outline/course-moodle/{$this->getId()}/questions");
    }
    return $summary;
  }

  function poll() {
    global $user;
    global $_course_moodle_prefix;
    $uid = $user->uid;

    $requirement = $this->getOptions();
    //'Evaluation'
    if ($requirement['instance']) {
      if (!$moodle_course_module = course_moodle_get_coursemodule_from_id('questionnaire', $requirement['instance'])) {
        //error("There is no coursemodule with id $id");
      }
      if (!$moodle_questionnaire = course_moodle_moodle_get_record("questionnaire", "id", $moodle_course_module->instance)) {
        //error("The Moodle questionnaire with id $moodle_course_module->instance corresponding to this coursemodule $requirement['instance'] is missing");
      }
    }

    //check for questionnaire submission


    $sql = "SELECT mq.* FROM {$_course_moodle_prefix}questionnaire_attempts mq, {$_course_moodle_prefix}user mu
      WHERE mq.userid = mu.id AND qid = :quid AND mu.idnumber = :uid";
    $results_check_questionnaire_check = db_query($sql, array(':quid' => $moodle_questionnaire->id, ':uid' => $uid));
    while ($row = $results_check_questionnaire_check->fetch(PDO::FETCH_ASSOC)) {
      $array_check_questionnaire = $row;
    }

    if (is_array($array_check_questionnaire)) {
      $this->getFulfillment()->setComplete(1)->save();
    }
  }

  public function getTakeUrl() {
    return url('moodle/mod/questionnaire/view.php', array('purl' => array('disabled' => TRUE), 'query' => array('id' => $this->getInstanceId())));
  }

  function getSummaryCSV() {
    $instance = $this->getMoodleInstanceId();
    // Get array of Drupal user IDs from Moodle CSV
    $users = array();
    $csv = array();
    $url = url('moodle/mod/questionnaire/report.php', array('query' => "instance=$instance&action=dcsv", 'absolute' => TRUE));
    $response = $this->getHttpRequestAsMoodleAdmin($url);
    $text = explode("\n", trim($response->data));
    foreach ($text as $key => $line) {
      if ($key) {
        // Any line other than the first line that doesn't have a response ID
        // should be merged with the header row.
        if (!preg_match('#^\d+\t#', $line)) {
          $text[0] .= $line;
          unset($text[$key]);
        }
      }
    }

    foreach ($text as $line) {
      $fields = explode("\t", $line);
      $fields = array_map('trim', $fields);

      if (!$skip) {
        $headers = $fields;
        $skip = TRUE;
      }
      else {
        $fields = array_pad($fields, count($headers), '');
        $newline = array_combine($headers, $fields);
        $keep = array(
          'Response',
          'Submitted on:',
          'Course',
          'ID',
        );
        foreach ($newline as $key => &$field) {
          if (!in_array($key, $keep) && strpos($key, 'Q') !== 0) {
            unset($newline[$key]);
          }
        }
        $csv[$newline['Response']] = $newline;
        $users[$newline['ID']] = NULL;
      }
    }

    // get drupal IDs from array of moodle IDs in keys
    // $users is an array of drupalID=>moodleID
    $values = array_filter(array_keys($users));
    $sql = "select id, idnumber from {mdl_user} u where id in (:ids)";
    $result = db_query($sql, array(':ids' => $values));
    while ($row = $result->fetch()) {
      $users[$row->id] = $row->idnumber;
    }

    foreach ($csv as &$row) {
      $row['idnumber'] = $users[$row['ID']];
    }

    return $csv;
  }

  function getReports() {
    return array(
      'overview' => array(
        'title' => 'Summary',
        'default' => TRUE,
        'pdf' => TRUE,
        'children' => array(
          'csv' => array(
            'title' => 'Export with profile data',
            'file' => TRUE,
          ),
        ),
      ),
    );
  }

  function getReport($key, $subkey) {
    if ($key == 'overview' && !$subkey) {
      $report = $this->getReportInfo();
      return array(
        'title' => $reports[$key]['title'],
        'url' => $report['url'],
      );
    }
    if ($key == 'overview' && $subkey == 'csv') {
      $this->renderOverview();
    }
  }

  function renderOverview() {
    $mcsv = $this->getSummaryCSV();
    $dcsv = $this->attachProfile($mcsv);
    $this->renderCSV($dcsv);
  }

  function getMoodleQuestionsEditUrl() {
    return url('moodle/mod/questionnaire/questions.php', array('purl' => array('disabled' => TRUE), 'query' => array('id' => $this->getInstanceId())));
  }

}

class CourseObjectMoodleScorm extends CourseObjectMoodle {

  function poll() {
    global $user;
    global $_course_moodle_prefix;
    $uid = $user->uid;

    $requirement = $this->getOptions();
    $node = node_load($requirement['nid']);
    //'SCORM/AICC'
    $sql_get_scorms = "SELECT * FROM {$_course_moodle_prefix}scorm WHERE course = :cid";
    $results = db_query($sql_get_scorms, array(':cid' => $node->course_moodle['cid']));

    while ($row = $results->fetch(PDO::FETCH_ASSOC)) {
      $scormid = $row['id'];
    }

    $moodle_user = course_moodle_get_moodle_user($uid);

    if ($scormid) {
      //check the scorm records for the user
      $tracked_scorm_complete = $this->scormtrack($moodle_user['id'], $scormid);

      if (is_array($tracked_scorm_complete) && $tracked_scorm_complete['complete']) {
        $this->getFulfillment()->setComplete(1)->save();
      }
    }
  }

  /**
   * Track a SCORM object.
   */
  function scormtrack($userid, $scormid) {
    global $_course_moodle_prefix;


    $res = db_query("SELECT * FROM {$_course_moodle_prefix}scorm WHERE id = :scoid", array(':scoid' => $scormid));
    while ($row = $res->fetch()) {
      $scorm = $row;
    }

    $SQL = "SELECT * FROM {$_course_moodle_prefix}scorm_scoes_track WHERE element LIKE :like AND (value like :passed OR value like :finished OR value like :finished)  AND scormid = :scormid AND userid = :userid";
    $resq = db_query($SQL, array(':like' => '%lesson_status%', ':complete' => 'complete%', ':passed' => 'passed%', ':finished' => 'finished%', ':scormid' => $scormid, ':userid' => $userid));
    while ($row = $resq->fetch()) {
      $scormtrack = $row;
    }

    if (is_object($scormtrack)) {
      $date_completed = NULL;
      $complete = NULL;

      if (trim($scormtrack->value) == 'passed' || trim($scormtrack->value) == 'completed' || trim($scormtrack->value) == 'complete' || trim($scormtrack->value) == 'finished') {
        $date_completed = $scormtrack->timemodified;
        $complete = 1;
      }
      elseif ($scormtrack->value == 'incomplete') {
        $date_completed = NULL;
        $complete = 0;
      }
    }
    else {
      $date_completed = NULL;
      $complete = NULL;
    }

    if ($scormtrack->id) {
      //echo "track";
    }
    return array(
      'date_completed' => $date_completed,
      'complete' => $complete,
    );
  }

  public function getTakeUrl() {
    return url('moodle/mod/scorm/view.php', array('purl' => array('disabled' => TRUE), 'query' => array('id' => $this->getInstanceId())));
  }

  function getReport($key) {
    if ($key == 'overview') {
      $report = $this->getReportInfo();
      return array(
        'url' => $report['url'],
      );
    }
  }

  function getReports() {
    return array(
      'overview' => array(
        'title' => 'SCORM Report',
        'default' => TRUE,
      ),
    );
  }

  function getMoodleQuestionsEditUrl() {
    return course_moodle_path(TRUE) . "/course/modedit.php?section=0&add=scorm&course={$this->getCourse()->getNode()->course_moodle['cid']}";
  }

}
