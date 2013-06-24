<?php
/**
 * list of vocabularies, which link to Taxonomy Manager interface
 */
function yaouseit_student_course_notify() {
	$output = l(t('Add new vocabulary'), 'admin/structure/taxonomy/add/vocabulary') . ' | ';
	$output .= l(t('Edit vocabulary settings'), 'admin/structure/taxonomy') . '<br /><br />';

	$vocabularies = taxonomy_get_vocabularies();
	$voc_list = array();

	foreach ($vocabularies as $vocabulary) {
		$voc_list[] = l($vocabulary->name, 'admin/structure/taxonomy_manager/voc/' . $vocabulary->machine_name);
	}
	if (!count($voc_list)) {
		$voc_list[] = t('No Vocabularies available');
	}
	$output .= theme('item_list', array('items' => $voc_list, 'title' => t('Vocabularies')));
	return $output;
}
/**
 * get my course of today
 */
function yaouseit_get_my_today_courses() {
	GLOBAL $user;
	$conditions = array(
			's_uid'=> $user->uid,
			'date'=> date('Y-m-d')
			);
	//print_r('and con='.$conditions);
	
	$my_courses = yaouseit_get_courses('coming',$conditions);
	//print_r(count($my_courses).'and con='.count($conditions));
	//die($my_courses." and con = ". $conditions);
	
	if(count($my_courses)){
		$output = t('please notice: you have class(s) today!');
		print_r($output);
		//make it to a list
		$tetequ_check_code=tetequ_check_code('1useit','7-201');
		foreach($my_courses as $student_uid=> $course){
			$title = 'testing'; //need to get course's name
			$role = 2;
			$name = $user->name;
			$room_no = "7-201";
			$user_id = $student_uid;
		
		$form .= <<<TT

<form action="http://www.tetequ.com/api/outer" method="post">
		
name:<input type=hidden name="name" value="{$name}"></input>
role:<input type=hidden name="role" value="{$role}"></input>
room_title:<input type=hidden name="room_title" value="{$title}"></input>
from:<input type=hidden name="from" value="1useit"></input>
check_code:<input type=hidden name="check_code" value="{$tetequ_check_code}"></input>
room:<input type=hidden name="room" value="{$room_no}"></input>
userid:<input type=hidden name="userid" value="{$user_id}"></input>
<input type=submit value="outer">
</form>
		
TT;
		
			}
	//die($form);
	return array('#markup' => $form);
	
	}
		
}

/**
 * list of courses according to conditions.
 * @param $type including: coming, past, all
 * @param $conditions 
 */
function yaouseit_get_courses($type,$conditions) {
	/*
	GLOBAL $user;
	$is_self = 0;
	if($conditions['s_uid'] == $user->uid) $is_self = 1;
	*/
	$today = date('Y-m-d');
	//switch to the course start time, then course no
	//if coming, the conditioins are date =today and course no > current course no or date > today.
	//if past, the conditions are date =today and course no < current course no or date < today
	//if type is null or all, get all 
	$current_course_no = yaouseit_get_courseno(date('Hi'));
	//die('get course no='.$current_course_no);
	$where = '';
	if(is_array($conditions))  $where = get_where($conditions);
	
	if($type =='coming') {
		$where .=" and ((date ='$today' and course_no >$current_course_no) or date>'$today' )" ;
		$course_message = t('The coming courses are:'); 
	}
	elseif ($type =='past') {
		$where .="((date ='$today' and course_no <$current_course_no ) or date<'$today' )" ;
		$course_message = t('The past courses are:');
	}else
		$course_message = t('All courses are:');

	$where .= "  order by s_uid, date, course_no ";
	$where = "where 1 ".$where;
	
	//die('where is '.$where );
	$arr_student_booking = array();
	
	$result = db_query("SELECT * FROM {user_refer_course} ". $where);
	
	while ($record = $result->fetchObject()) {
		$arr_student_booking[$record->s_uid] = array(
				'date' => $record->date,
				'course_no'=>  $record->course_no,
				'course_start_time' => $record->course_start_time,
				'course_end_time' => $record->course_end_time,
				't_uid' => $record->t_uid 
				);
	}
	return $arr_student_booking;
	/*
	if(!count($arr_student_booking) ){
		$output = l(t('Add new vocabulary'), 'admin/structure/taxonomy/add/vocabulary') . ' | ';
	}
	$output = l(t('Add new vocabulary'), 'admin/structure/taxonomy/add/vocabulary') . ' | ';
	$output .= l(t('Edit vocabulary settings'), 'admin/structure/taxonomy') . '<br /><br />';

	$vocabularies = taxonomy_get_vocabularies();
	$voc_list = array();

	foreach ($vocabularies as $vocabulary) {
		$voc_list[] = l($vocabulary->name, 'admin/structure/taxonomy_manager/voc/' . $vocabulary->machine_name);
	}
	if (!count($voc_list)) {
		$voc_list[] = t('No Vocabularies available');
	}
	
	$output .= theme('item_list', array('items' => $voc_list, 'title' => t('Vocabularies')));
	return $output;
	*/
}

//Éú³Éwhere×Ö·û´®
function get_where($arg = null) {
	foreach ((array)$arg as $key => $val) {
		if(is_int($key)) {
			$where .= " $val ";
		}else {
			if(is_string($val)) {
				if($val === null) {
					$where .= " and $key is null ";
				}else {
					$where .= " and $key = '$val' ";
				}
			}elseif(is_array($val)) {
				foreach ($val as $v) {
					if(is_string($v)) {
						$in .= $in ? ",'$v'" : "'$v'";
					}else {
						$in .= $in ? ",$v" : "$v";
					}
				}
				$where .= " and $key in ($in)";
			}else {
				$where .= " and $key = $val ";
			}
		}
	}
	return $where;
}

