<?php


/**
 * Implements hook_menu().
 *
 * Here we set up the URLs (menu entries) for the
 * form examples. Note that most of the menu items
 * have page callbacks and page arguments set, with
 * page arguments set to be functions in external files.
 */
function yaouseit_menu() {
  $items = array();
  $items['admin/config/yaouseit'] = array(
    'title'            => '1useit.com settings',
    'page callback'    => 'drupal_get_form',
    'page arguments'   => array('yaouseit_admin_settings'),
    'access arguments' => array('administer site configuration'),
    'file'             => 'yaouseit.admin.inc',
  );

  $items['course/booking'] = array(
    'title' => 'Course booking',
    'page callback' => 'yaouseit_course_booking',
    'access callback' => TRUE,
    'expanded' => TRUE,
    'file' => 'yaouseit_course.inc',
  );
  $items['course/booking/states'] = array(
    'title' => 'My course booking states',
    'page callback' => 'yaouseit_course_booking',
    //'page arguments' => array('yaouseit_states_form'),
    'access callback' => 'user_is_logged_in',
    'file' => 'yaouseit_course.inc',
  );
   $items['course/classroom'] = array(
    'title' => 'Live Classroom',
    'page callback' => 'yaouseit_enter_classroom',
    'access callback' => TRUE,
    'expanded' => TRUE,
    'file' => 'yaouseit_classroom.inc',
  );
  return $items;
}

function yaouseit_term_compare($a, $b)
{
    //$order = -1; // This is just to reverse the natural sort order (ie make it descending)
    // if you want a case sensitive sort, swap out strcasecmp() for strcmp()
    return intval($a->name) > intval($b->name) ? 1 : 0;
    //return $order * strcasecmp($a->name, $b->name);
}

/*
 * @return
Array
(
	[1] => Array
	(
		[0] => 9:00
		[1] => 9:25
	)

	[2] => Array
	(
		[0] => 9:30
		[1] => 9:55
	)
......
)
*/
function yaouseit_get_course_schedule() {
	$arr = taxonomy_get_tree(variable_get('yaouseit_course_schedule_vid', 7), $parent = 0, $max_depth = NULL, $load_entities = FALSE) ;
	if(empty($arr))return array();
	
	foreach($arr as $one){
	if( 0 == $one->parents[0] ){
		$arr1[] = $one;
	}
	}
	usort($arr1, "yaouseit_term_compare");
	//print_r($arr1);
	$tree = array();
	foreach($arr1 as $one1){
	foreach($arr as $one){
		if( $one1->tid == $one->parents[0] ){
			$tree[$one1->name][] = $one->name;
		}
	}
	}
	//print_r($tree);
	return $tree;
}

function tetequ_check_code($company_code,$roomid){
	//return md5(base64_encode("{roomid:".$roomid.",key:".$company_code."}"));
	return '1useit519c3eee191a3';
}


/*
 * @return
*/
function yaouseit_get_jiao_cai_course() {
	//$courses = taxonomy_get_tree(variable_get('yaouseit_en_course_vid', 5), $parent = 0, $max_depth = NULL, $load_entities = FALSE) ;
	//print_r($courses);die;
	$jiao_cai = taxonomy_get_tree(variable_get('yaouseit_jiao_cai_vid', 4), $parent = 0, $max_depth = NULL, $load_entities = FALSE) ;
	//print_r($jiao_cai);die;
/*
[9] => stdClass Object
        (
            [tid] => 91
            [vid] => 4
            [name] => ��ˉ�介��辫����1
            [description] => 
            [format] => 
            [weight] => 1
            [depth] => 3
            [parents] => Array
                (
                    [0] => 27
                )

        )
*/
	$arr = taxonomy_get_tree(variable_get('yaouseit_jiao_cai_course_vid', 9), $parent = 0, $max_depth = NULL, $load_entities = FALSE) ;
	if(empty($arr))return array();
	//print_r($arr);die;
	foreach($arr as $one){
	if( 0 == $one->parents[0] ){
		$arr1[] = $one;
	}
	}
	//print_r($arr1);
	$tree = array();
	foreach($arr1 as $one1){
	foreach($arr as $one){
		if( $one1->tid == $one->parents[0] ){
			foreach($jiao_cai as $one2){
				if( $one1->name == $one2->name ){
					$tree[$one2->tid]['name'] = $one1->name;
					$tree[$one2->tid]['courses'][$one->tid] = $one->name;
				}
			}
		}
	}
	}
	foreach($tree as $k=>$arr2){
		ksort($tree[$k]['courses']);
	}
	//print_r($tree);die;
/*
Array
(
    [91] => Array
        (
            [name] => ��ˉ�介��辫����1
            [courses] => Array
                (
                    [491] => 绗��璇�                    [497] => 绗��璇�                    [493] => 绗��璇�                    [499] => 绗��璇�                    [492] => 绗��璇�                    [495] => 绗��璇�                    [498] => 绗��璇�                    [496] => 绗��璇�                    [501] => 绗��涓��
                    [502] => 绗��浜��
                    [500] => 绗��璇�                    [494] => 绗��璇�                )

        )
*/
	return $tree;
}


function yaouseit_cron() {
	global $user;

	$date=date('Y-m-d');

	//$hm = intval(date('Hi'));
	$h=intval(date('H')+variable_get('yaouseit_booking_no_cancel_time',5));
	$h= $h<24 ? $h : 23;
	$hm2 = intval( $h . date('i') );
	$max_course_no=0;
	$schedule = yaouseit_get_course_schedule();
	foreach($schedule as $no => $one){	//1 2 ... 28
		if( intval(preg_replace('/[^\d]/','',$one[0])) <= $hm2 ){
			$max_course_no = $no;
			//break;
		}
	}
	$where1 = 'flag=0 AND ';
	$max_date = date('Y-m-d', mktime(0,0,0,date("m"),date("d")-1,date("Y")) );	
	if($max_course_no){
		$sql="SELECT * FROM {user_refer_course} r WHERE $where1 (date<='$max_date' OR (date='$date' AND r.course_start_time<$max_course_no)) AND realm='student_booking' ORDER BY r.`date`,r.course_start_time,r.t_uid,r.s_uid";
	}
	else{
		$sql="SELECT * FROM {user_refer_course} r WHERE $where1 date<='".$max_date . "' AND realm='student_booking' ORDER BY r.`date`,r.course_start_time,r.t_uid,r.s_uid";
	}

	$arr_class =$arr_uids= array();
	//die($sql);
	$result = db_query($sql);
	while ($record = $result->fetchObject()) {
		$arr_class[$record->date][$record->course_start_time][$record->t_uid]['s_uids'][] = $record->s_uid;
		$arr_class[$record->date][$record->course_start_time][$record->t_uid]['course_no'] = $record->course_no;
		$arr_uids[$record->t_uid]=$record->t_uid;
		//$arr_course_no[]=$record->course_no;
	}
	if( empty($arr_class) )return;
	
	$arr_uids = array_keys($arr_uids);
	//print_r($arr_class);print_r($arr_uids);die;

	$arr_user = array();
	$sql="SELECT * FROM {users} WHERE uid in (".join(',',$arr_uids) . ")";
	//die($sql."<br>\n");
	$result = db_query($sql);
	while ($record = $result->fetchObject()) {
		$arr_user[$record->uid]=$record;
	}
	
	$jiao_cai_course = yaouseit_get_jiao_cai_course();//print_r($jiao_cai_course);die;

	$arr_prices = array();
	$sql="SELECT * FROM {user_refer_course} WHERE realm='prices' AND t_uid in (".join(',',$arr_uids) . ")";
	$result = db_query($sql);
	while ($record = $result->fetchObject()) {
		$arr_prices[$record->t_uid][$record->course_no] = $record->course_start_time;
	}//print_r($arr_prices);die;

	$orig_schedule = taxonomy_get_tree(variable_get('yaouseit_course_schedule_vid', 7), $parent = 0, $max_depth = NULL, $load_entities = FALSE) ;

	foreach($arr_class as $adate => $arr_no){
	foreach($arr_no as $no=>$arr_t_uid){
	foreach($arr_t_uid as $t_uid=>$arr_t){//print_r($arr_no);print_r($arr_t_uid);print_r($arr_t);
		$class_node = new stdClass();
		$class_node->type = 'class';
		$class_node->title    = REQUEST_TIME;//pending
		$class_node->language = LANGUAGE_NONE;
		$class_node->status = 1;
		$class_node->uid = 1;
		$class_node->validated=1;
		$class_node->is_new=1;

		foreach($jiao_cai_course as $j => $arr){ //86 87 91 92
			foreach($arr['courses'] as $c_tid=> $c_name){
				if($c_tid==$arr_t['course_no']){
					$class_node->title =$arr['name'].':'.$c_name.'-'.$arr_user[$t_uid]->name;
					break;
				}
			}
		}

		$class_node->field_date[LANGUAGE_NONE][] = array(
		'value'  => "$adate ".$schedule[$no][0].':00',
		'value2' => "$adate ".$schedule[$no][1].':00',
		);  

		$class_node->field_teacher[LANGUAGE_NONE][] = Array
		(
			'uid' => $t_uid,
		'target_id' => $t_uid,
		);

		foreach($arr_t['s_uids'] as $s_uid){
			$class_node->field_student[LANGUAGE_NONE][] = Array
			(
				'uid' => $s_uid,
			'target_id' => $s_uid,
			);
			if (module_exists('userpoints')) {
				$points=variable_get('yaouseit_default_class_points', 150);
				foreach($jiao_cai_course as $j => $arr){ //86 87 91 92
					foreach($arr['courses'] as $c_tid=> $c_name){
						if($c_tid==$arr_t['course_no']){
							$points = isset($arr_prices[$t_uid][$j]) ? $arr_prices[$t_uid][$j] : $points;
							break;
						}
					}
				}//var_dump($points);
				$params = array (
				  'uid' => $s_uid,
				  'points' => -1*$points,
				);
				if($points)userpoints_userpointsapi($params);
			}
		}

		$class_node->field_class_lesson[LANGUAGE_NONE][] = Array
		(
			'tid' => $arr_t['course_no'],
		'target_id' => $arr_t['course_no'],
		);

	
		foreach($orig_schedule as $one){
		if( 0 == $one->parents[0] ){
			if($no==$one->name){
				$class_node->field_course_schedule[LANGUAGE_NONE][] = Array
				(
					'tid' => $one->tid,
				'target_id' => $one->tid,
				);
			}
		}
		}


		//print_r($class_node);
		if(1 && $class_node = node_submit($class_node)) {
		node_save($class_node);
		$order_nid = $class_node->nid;
		//ALTER TABLE  `user_refer_course` ADD  `flag` INT NOT NULL DEFAULT  '0'
		$sql="UPDATE {user_refer_course} SET flag=1 WHERE realm='student_booking' "
				. " AND t_uid='".$t_uid . "' AND date='".$adate . "' AND course_start_time='".$no . "' AND course_no='".$arr_t['course_no']."'" ;
		//var_dump($sql);
		$result = db_query($sql);
		
		}
	}
	}
	}
}

function _yaouseit_node_presave($node) {
  print_r($node);die;
}
/*

stdClass Object
(
    [uid] => 3
    [name] => guolii
    [type] => class
    [language] => en
    [title] => class1
    [status] => 1
    [promote] => 0
    [sticky] => 0
    [created] => 1369180888
    [revision] => 0
    [comment] => 2
    [menu] => Array
        (
            [enabled] => 0
            [mlid] => 0
            [module] => menu
            [hidden] => 0
            [has_children] => 0
            [customized] => 1
            [options] => Array
                (
                    [attributes] => Array
                        (
                            [title] => 
                            [id] => 
                            [name] => 
                            [rel] => 
                            [class] => 
                            [style] => 
                            [target] => 
                            [accesskey] => 
                        )

                )

            [expanded] => 0
            [parent_depth_limit] => 8
            [link_title] => 
            [description] => 
            [parent] => main-menu:0
            [weight] => 0
            [language] => und
            [plid] => 0
            [menu_name] => main-menu
        )

    [nid] => 
    [vid] => 
    [changed] => 1369180888
    [additional_settings__active_tab] => 
    [log] => 
    [date] => 
    [submit] => Save
    [preview] => Preview
    [form_build_id] => form-25jLizMl25qC6hZTJXfDk_usbtIZmyYYly87w5hMF0M
    [form_token] => ufVHZeYOrjeKHkeiAVQW9W32gh9mF1RAHOMlpB3KSYM
    [form_id] => class_node_form
    [metatags] => Array
        (
        )

    [path] => Array
        (
            [alias] => 
            [pid] => 
            [source] => 
            [language] => und
        )

    [op] => Save
    [body] => Array
        (
            [und] => Array
                (
                    [0] => Array
                        (
                            [summary] => 
                            [value] => <p>body1</p>

                            [format] => filtered_html
                        )

                )

        )

    [field_date] => Array
        (
            [und] => Array
                (
                    [0] => Array
                        (
                            [value] => 2013-05-22 01:00:00
                            [value2] => 2013-05-22 01:30:00
                            [show_todate] => 1
                            [timezone] => Asia/Hong_Kong
                            [offset] => 28800
                            [offset2] => 28800
                            [timezone_db] => UTC
                            [date_type] => datetime
                        )

                )

        )

    [field_teacher] => Array
        (
            [und] => Array
                (
                    [0] => Array
                        (
                            [uid] => 7
                        )

                )

        )

    [field_student] => Array
        (
            [und] => Array
                (
                    [0] => Array
                        (
                            [uid] => 6
                        )

                )

        )

    [field_class_lesson] => Array
        (
            [und] => Array
                (
                    [0] => Array
                        (
                            [tid] => 3575
                        )

                )

        )

    [field_course_schedule] => Array
        (
            [und] => Array
                (
                    [0] => Array
                        (
                            [tid] => 3286
                        )

                )

        )

    [validated] => 1
    [is_new] => 1
    [timestamp] => 1369180888
)
*/
/**
 * @} End of "defgroup yaouseit".
 */
