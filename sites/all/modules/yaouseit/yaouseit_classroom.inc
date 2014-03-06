<?php

/**
 * get my course of today and the coming
 */
function yaouseit_get_my_coming_courses() {
	GLOBAL $user;
	$conditions = array (
			's_uid' => $user->uid,
			'date' => date ( 'Y-m-d' ) 
	);
	// print_r('and con='.$conditions);
	
	$courses_today = yaouseit_get_courses ( 'today', $conditions );
	$conditions = array (
			's_uid' => $user->uid,
			'date' => date ( 'Y-m-d', mktime ( 0, 0, 0, date ( "m" ), date ( "d" ) + 1, date ( "Y" ) ) ) 
	);
	$courses_coming = yaouseit_get_courses ( 'coming', $conditions ); // courses
	                                                                  // of
	                                                                  // coming,
	                                                                  // not
	                                                                  // including
	                                                                  // today
	
	$link_html = "";
	if (count ( $courses_today )) {
		$output = t ( 'please notice: you have class(s) today!' );
		print_r ( $output );
		$link_html = make_html_for_courses ( $courses_today );
	}
	$link_html .= make_html_for_courses ( $courses_coming );
	return array (
			'#markup' => $link_html 
	);
}

/**
 * make html for courses
 */
function make_html_for_courses($courses_today) {
	$link_html = "";
	foreach ( $courses_today as $no => $course ) {
		// *****************************//
		$term = taxonomy_term_load ( $course->course_no );
		
		$title = $term->name; // need to get course's name
		                      // $lesson_arr_time = taxonomy_get_tree (
		                      // $course->course_no, $parent = 0, $max_depth =
		                      // NULL,
		                      // $load_entities = FALSE );
		$course_schedule = yaouseit_get_course_schedule ();
		
		// *****************************//
		// get last lesson end time, if now < last lesson end time, still in
		// last time, or, new lesson will start.
		// $last_lesson_arr_time = taxonomy_get_tree ( ($course->course_no - 1),
		// $parent = 0, $max_depth = NULL, $load_entities = FALSE );
		// print_r($course);
		// if the lesson is the first one, enter the classroom previously 10
		// minutes
		if ($course->course_start_time == 1) {
			$last_lesson_end_time = strtotime ( $course_schedule [$course->course_start_time] [0] ) - 10 * 60;
		} else {
			$last_lesson_end_time = strtotime ( $course_schedule [$course->course_start_time - 1] [1] );
		}
		
		$teacher = user_load ( $course->t_uid );
		
		$teacher_name = $teacher->name;
		// if last lesson end time < now, enter this lesson,(your lession is
		// coming, join it), or enter the teacher's classroom (your lesson
		// hasn't start, join other's lesson)
		if ($last_lesson_end_time > time ()) {
			$link_html .= "<a href=/classroom/" . $course->t_uid . "> your lesson hasn't started, look at what the teacher: " . $teacher_name . " is teaching" . " </a>";
		} else {
			$link_html .= "<a href=/classroom/" . $course->t_uid . "> your lesson is starting, enter classroom" . "</a>";
		}
	}
	return $link_html;
}

/*
 * enter classroom of teacher --/classroom/{$user->uid}. for example
 * /classroom/13 /classroom/ list current live classroom; /classrom/13 enter the
 * current classroom of user 13 1.get course_no according to time, 2.get the
 * bookers, and their info: name, lesson history, and etc. 3.get the course
 * info: lesson 4.check the user's role--teacher, booker, non-booker. 5.enter
 * the classroom, with the parameters
 */
function yaouseit_enter_classroom($account) {
	//$person_id = preg_replace ( '/classroom\//', '', $_GET ['q'] );
	//$person_id = preg_replace ( '/classroom/', '', $person_id );
	return enter_personal_room ( $account->uid);

}
function enter_personal_room($person_id) {
	GLOBAL $user;
	
	$person = user_load ( $person_id );
	if (! $person)
		return '';
	
	$role_type = '';
	print_r("pernon's name is ".$person->name);
	// check the user role, if not teacher, just enter his classroom without
	// other params and not record video,else, enter the teacher classroom with
	// params
	
	$room_role = 0;
	if ($person_id == $user->uid)
		$room_role = 1;
	//$title = $person->name . ' 的教室 ';
	$title = format_username($person). '  的教室' ;
	
	if (in_array ( 'teacher', array_values ( $person->roles ) )) { // teacher
		$role_type = 'teacher';
		
		// 1.get course no.
		$course_no = yaouseit_get_courseno ( date ( 'Hi' ) );
		// if no course_no, all courses are over!
		if ($course_no) {
			
			// 2. get bookers
			$courses = yaouseit_get_courses ( 'today', array (
					'course_start_time' => $course_no,
					't_uid' => $person_id,
					'date' => date ( 'Y-m-d' ) 
			) );
			// print_r($courses);
			// die;
			$students = array ();
			
			if (count ( $courses )) {
				foreach ( $courses as $no => $course ) {
					$students [] = user_load ( $course->s_uid );
					$term_id = $course->course_no;
					
					// 3. get the course info：title
					$course_info = taxonomy_term_load ( $term_id );
					$title = $course_info->name;
				}
			}
			
			$room_username = format_username($user);
			$avatar = '';
			
			// if booker, or self
			
			if (! $room_role) {
				foreach ( $students as $student ) {
					if ($student->uid == $user->uid) {
						$room_role = 2;
						// if booker, use his avatar, or, use default booker's
						// avator
						// need to be updated.
						$picture = $user->picture;
						$preset ['presetname'] = 'thumbnail';
						$pic_booker = "";
						$alt = isset ( $user->picture->filename ) ? $user->picture->filename : $pic_booker;
						
						$attributes = array (
								'attributes' => array (
										'title' => 'Profile',
										'class' => 'bt_' . $user->uid 
								),
								'html' => TRUE 
						);
						$image = theme ( 'imagecache', $preset ['presetname'], $picture, $alt );
						$avatar = l ( $image, "user/$user->uid", $attributes );
						break;
					}
				}
			}
		}
	} else if (in_array ( 'student', array_values ( $person->roles ) )) { // student
		if ($user->uid == $account->uid)
			$room_role = 1;
		else
			$room_role = 2;
	}
	
	if (! $room_role)
		$room_role = 2; // if not booked,teacher, role should be 3, but now is 2;
	
	$room = yaouseit_get_roomno ( array (
			'person' => $person_id,
			'course_no' => $course_no 
	) );
	
	print_r("\n before entering room");
	
	return enter_tetequ_room ( array (
			'role' => $room_role,
			'room' => $room,
			'avatar' =>$avatar,
			'title' => $title 
	) );
}

// enter tetequ's room
function enter_tetequ_room($arr) {
	global $user;

	print_r("enter enter_tetequ_room");
	$avatar = isset ( $arr ['avatar'] ) ? $arr ['avatar'] : '' ;
	$title = isset ( $arr ['title'] ) ? $arr ['title'] : ' 1useit.com ' ;
	$room_role = isset ( $arr ['role'] ) ? $arr ['role'] : 2 ;

	$tetequ_live_code = tetequ_live_code ();
	$company_code = "1useit";
	$room_username = format_username($user);
	$form = <<<TT
<script language=javascript> 
setTimeout("document.form1.submit()",0) 
</script> 	
<form name="form1" action="http://www.tetequ.com/api/agency_live" method="post">
<input type=hidden name="name" value="{$room_username}"></input>
<input type=hidden name="userid" value="{$user->uid}"></input>
<input type=hidden name="avator" value="{$avatar}"></input>

<input type=hidden name="live_code" value="{$tetequ_live_code}"></input>
<input type=hidden name="role" value="{$room_role}"></input>
<input type=hidden name="room" value="{$arr['room']}"></input>
<input type=hidden name="room_title" value="{$title}"></input>
<input type=hidden name="company_code" value="{$company_code}"></input>
<input type=submit value="outer">
</form>
		
TT;
	
	// die($form);
	return array (
			'#markup' => $form 
	);
}
function yaouseit_record_video($account, $type, $node) {
	global $user;
	global $base_url;
	print_r ( "type  is " . $type );
	if (!$type) { // user
		return enter_personal_room ( $account->uid );
	} else {  //node, comment ....
		$room_role = 2;
		if ($node->uid == $account->uid)
			$room_role = 1;
		
		if ($type == 'node') {
			$arr = array (
					'person' => $account->uid,
					'for_type' => $type,
					'for_no' => $node->nid,
					'title' =>$node->title
			);
			$room = yaouseit_get_roomno ( $arr );
		} else if ($type == 'comment') {
			$arr = array (
					'person' => $account->uid,
					'for_type' => $type,
					'for_no' =>  $node->cid,
					'title' => $node->subject 
			);
			$room = yaouseit_get_roomno ( $arr );
		}
		$arr_tetequ = array (
				'role' => $room_role,
				'room' => $room,
				'title' =>  $arr['title'] 
		);
		return enter_tetequ_room ( $arr_tetequ );
	}
}
function tetequ_live_code() {
	require_once ("agency.php");
	$agency = new agency ();
	
	$client_info = array (
			'key' => tetequ_check_code ( '', '' ),
			'company_code' => "1useit" 
	); // 必填
	   // 录播回放接口地址
	   // $replay_url = 'http://www.tetequ.com/api/agency_replay';
	   // 获取直播接入码
	$json_live_code = $agency->get_live_code ( $client_info ['key'], $client_info ['company_code'] );
	// print_r('live code is '. $json_live_code);
	// die();
	// 得到的是json格式的数据
	$live_code = json_decode ( $json_live_code );
	// $live_code[status]如果是1，2，3，4都是出错，打印错误；如果是5，则正确，返回直播码；
	if ($live_code->status == 5) {
		// 设置直播码，传送到直播接口
		return $live_code->live_code;
	} else
		return 0;
}
/*
 * note: waiting for the class note. replay the video of classroom，
 * /video/{teacher_id}/{room_no} @params room_no is like 20130523-01 1.get
 * filters: date, textbook, lesson. 2.get lessons history according to userid.
 * 3.
 */
function yaouseit_video($teacher_id, $room_no = null) {
	global $user;
	
	$room_no = preg_replace ( '/video\//', '', $_GET ['q'] ); // video/3--2013-06-28--2
	$room_no = preg_replace ( '/video/', '', $room_no );
	$arr = explode ( '--', $room_no );
	$course_no = $arr [2];
	$course_date = $arr [1];
	$teacher_id = $arr [0];
	
	// if no teacher_id or room_no, list videos! else, replay
	if (! $teacher_id or ! $room_no) {
		// list videos and return， need to update
		return;
	}
	
	watchdog ( 'custom', '@type replayed video %title.', array (
			'@type' => $user->name,
			'%title' => "$course_date No.$course_no" 
	) );
	
	// the classroom no is complete, no need to make it
	// $tetequ_check_code = tetequ_check_code ('','');
	// print_r ( 'check_code is ' . $tetequ_check_code );
	$tetequ_live_code = tetequ_live_code ();
	
	// get course no from room no.
	// "1useit-" . $teacher_id . "-" . $date . "-" . $course_no;
	/*
	 * $arr = explode ( '-', $room_no ); $course_no = $arr [3]; $course_date =
	 * $arr [2]; $teacher_id = $arr [1]; $company_code = $arr [0];
	 */
	$company_code = "1useit";
	
	/*
	 * $courses = yaouseit_get_courses ( 'past', array ( 'course_start_time' =>
	 * $course_no, 't_uid' => $teacher_id, 'date' => $course_date ) ); $title =
	 * ''; if (count ( $courses )) { foreach ( $courses as $no => $course ) {
	 * //$students [] = user_load ( $course->s_uid ); $term_id =
	 * $course->course_no; // get the course info：title $course_info =
	 * taxonomy_term_load ( $term_id ); $title = $course_info->name; // need to
	 * get course's name } }
	 */
	$title = "$course_date No.$course_no";
	
	$form = <<<TT

<form name="form1" action="http://www.tetequ.com/api/agency_replay" method="post">
<input type=hidden name="live_code" value="{$tetequ_live_code}"></input>
<input type=hidden name="room" value="{$room_no}"></input>
<input type=hidden name="room_title" value="{$title}"></input>
<input type=hidden name="company_code" value="{$company_code}"></input>
<input type=submit value="play">
</form>
<script language=javascript> 
setTimeout("document.form1.submit()",0) 
</script> 	
		
TT;
	
	// die($form);
	return array (
			'#markup' => $form 
	);
}
/*
 * get course number according to the time
 */
function yaouseit_get_courseno($time) {
	$schedule = yaouseit_get_course_schedule ();
	// print_r("\n time is ". $time . " schedule is ".$schedule);
	// print_r($schedule);
	if (! $time)
		$hm = intval ( date ( 'Hi' ) );
	else
		$hm = intval ( $time ); // $hm = intval ( date ( 'Hi', $time ) );
	$last_end_time = 0;
	foreach ( $schedule as $no => $one ) { // 1 2 ... 28
	                                       // 时间或者刚好在同一节课之间，或者在两节课之间
		if ((intval ( preg_replace ( '/[^\d]/', '', $one [0] ) ) < $hm && intval ( preg_replace ( '/[^\d]/', '', $one [1] ) ) > $hm) || ($hm > $last_end_time && $hm < intval ( preg_replace ( '/[^\d]/', '', $one [0] ) ))) {
			return $no;
			break;
		}
		
		$last_end_time = intval ( preg_replace ( '/[^\d]/', '', $one [1] ) );
	}
}
/**
 * list of courses according to conditions.
 *
 * @param $type including:
 *        	today (coming of today),coming, past, all
 * @param
 *        	$conditions
 */
function yaouseit_get_courses($type, $conditions) {
	/*
	 * GLOBAL $user; $is_self = 0; if($conditions['s_uid'] == $user->uid)
	 * $is_self = 1;
	 */
	$today = date ( 'Y-m-d' );
	// switch to the course start time, then course no
	// if coming, the conditioins are date =today and course no > current course
	// no or date > today.
	// if past, the conditions are date =today and course no < current course no
	// or date < today
	// if type is null or all, get all
	$current_course_no = yaouseit_get_courseno ( date ( 'Hi' ) );
	// die('get course no='.$current_course_no);
	$where = '';
	if (is_array ( $conditions )) {
		// handle course_start_time
		if (isset ( $conditions ['course_start_time'] )) {
			$course_start_time = $conditions ['course_start_time'];
			unset ( $conditions ['course_start_time'] );
			$current_course_no = yaouseit_get_courseno ( $course_start_time );
		}
		
		$where = get_where ( $conditions );
	}
	
	if ($type == 'coming') {
		$where .= " and ((date ='$today' and (course_start_time + 0 ) >=$current_course_no ) or date>'$today' )";
		$course_message = t ( 'The coming courses are:' );
	} elseif ($type == 'today') {
		$where .= " and ((date ='$today' and (course_start_time + 0 ) >=$current_course_no) )";
		$course_message = t ( 'Today, you will have course(s) : ' );
	} elseif ($type == 'past') {
		$where .= "((date ='$today' and (course_start_time + 0 ) <$current_course_no ) or date<'$today' )";
		$course_message = t ( 'The past courses are:' );
	} else
		$course_message = t ( 'All courses are:' );
	
	$where .= "  order by s_uid, date, course_no ";
	$where = "where 1 " . $where;
	
	// die('\n where is '.$where );
	$arr_student_booking = array ();
	
	$result = db_query ( "SELECT * FROM {user_refer_course} " . $where );
	return $result->fetchAll ();
	/*
	 * while ($record = $result->fetchObject()) {
	 * $arr_student_booking[$record->s_uid] = array( 'date' => $record->date,
	 * 'course_no'=> $record->course_no, 'course_start_time' =>
	 * $record->course_start_time, 'course_end_time' =>
	 * $record->course_end_time, 't_uid' => $record->t_uid ); } return
	 * $arr_student_booking;
	 */
}

// 生成where字符串
function get_where($arg = null) {
	$where = '';
	foreach ( ( array ) $arg as $key => $val ) {
		if (is_int ( $key )) {
			$where .= " $val ";
		} else {
			if (is_string ( $val )) {
				if ($val === null) {
					$where .= " and $key is null ";
				} else {
					$where .= " and $key = '" . $val . "' ";
				}
			} elseif (is_array ( $val )) {
				foreach ( $val as $v ) {
					if (is_string ( $v )) {
						$in .= $in ? ",'$v'" : "'$v'";
					} else {
						$in .= $in ? ",$v" : "$v";
					}
				}
				$where .= " and $key in ($in)";
			} else {
				$where .= " and $key = $val ";
			}
		}
	}
	// print_r($where);
	// die;
	return $where;
}
/*
 * get room no.
 */
function yaouseit_get_roomno($arr) {
	// return md5(base64_encode("{roomid:".$roomid.",key:".$company_code."}"));
	if (! isset ( $arr ['person'] ))
		return null;
	
	$person_id = ( int ) $arr ['person'];
	$course_no = ( int ) (isset ( $arr ['course_no'] ) ? $arr ['course_no'] : 0);
	
	// $role = $arr ['role'];
	$for_type = $arr ['for_type'];
	$for_no = $arr ['for_no'];
	
	$date = isset ( $arr ['date'] ) ? date ( "Ymd", strtotime ( $arr ['date'] ) ) : date ( "Ymd" );
	$time = isset ( $arr ['time'] ) ? $arr ['time'] : date ( 'Hi' );
	
	if ($course_no > 0) {
		return "1useit--" . $person_id . "--" . $date . "--" . $course_no;
	} else {
		if ($for_type)
			return "1useit--" . $person_id . "--" . $date . "-" . $time . "--" . $for_type . "--" . $for_no;
		else
			return "1useit--" . $person_id . "--" . $date . "-" . $time;
	}
}
/*
 * get course no according to room_no
 */
function yaouseit_get_arr_from_roomno($str) {
	// "1useit-" . $teacher_id . "-" . $date . "-" . $course_no;
	// return substr(strrchr($str, "-"), 1);
	return explode ( '-', $str );
}
/*
 * get video accorrding to the params @params courseno, date, userid of teacher
 */
function yaouseit_get_video($arr) {
	$course_no = yaouseit_get_roomno ( $arr );
}
/*
 * function tetequ_check_code($company_code,$roomid){ //return
 * md5(base64_encode("{roomid:".$roomid.",key:".$company_code."}")); return
 * '1useit519c3eee191a3'; }
 */
