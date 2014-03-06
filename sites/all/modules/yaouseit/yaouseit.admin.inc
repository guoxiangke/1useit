<?php

/**
 * @file
 * Administration page callbacks for Moodle2 Integration.
 */

/**
 * Implements hook_admin_settings().
 */
function yaouseit_admin_settings() {	//yaouseit_cron();return;
  if('pricing'==preg_replace('/admin\/config\/yaouseit\//','',$_GET['q']) ){
  	return yaouseit_admin_price_settings();
  }

  $form['yaouseit_default_class_points'] = array(
    '#type'          => 'textfield',
    '#title'         => '默认的每节课程价格(单位：点数)',
    '#default_value' => variable_get('yaouseit_default_class_points', 300),
    '#size'          => 5,
    '#required'      => TRUE,
  );

  $form['yaouseit_price'] = array(
    '#type'          => 'item',
    '#title'         => '上课定价',
    '#markup'          => '<a href=/admin/config/yaouseit/pricing>点击这里</a>',
  );

  $form['yaouseit_jiao_cai_vid'] = array(
    '#type'          => 'textfield',
    '#title'         => 'Teaching material vid',
    '#default_value' => variable_get('yaouseit_jiao_cai_vid', 4),
    '#size'          => 2,
    '#required'      => TRUE,
  );
  $form['yaouseit_course_schedule_vid'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Course schedule vid'),
    '#default_value' => variable_get('yaouseit_course_schedule_vid', 7),
    '#size'          => 2,
    '#required'      => TRUE,
  );
  $form['yaouseit_jiao_cai_course_vid'] = array(
    '#type'          => 'textfield',
    '#title'         => 'Teaching material classes vid',
    '#default_value' => variable_get('yaouseit_jiao_cai_course_vid', 9),
    '#size'          => 2,
    '#required'      => TRUE,
  );

  $form['yaouseit_booking_no_cancel_time'] = array(
    '#type'          => 'textfield',
    '#title'         => 'Booking no cancel time(hours)',
    '#default_value' => variable_get('yaouseit_booking_no_cancel_time', 5),
    '#size'          => 2,
    '#required'      => TRUE,
  );

  $form['administrator_role_id'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Administrator role ID'),
    '#default_value' => variable_get('administrator_role_id', 3),
    '#size'          => 9,
    '#required'      => TRUE,
  );

  $form['student_role_id'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Student role ID'),
    '#default_value' => variable_get('student_role_id', 5),
    '#size'          => 9,
    '#required'      => TRUE,
  );

  $form['teacher_role_id'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Teacher role ID'),
    '#default_value' => variable_get('teacher_role_id', 6),
    '#size'          => 9,
    '#required'      => TRUE,
  );

  $form['aid_teacher_role_id'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Aid teacher role ID'),
    '#default_value' => variable_get('yaouseit_aid_teacher_rid', 7),
    '#size'          => 9,
    '#required'      => TRUE,
  );

  return system_settings_form($form);
}

function yaouseit_admin_price_settings(){
  global $base_url, $base_path, $base_root;

	$date=date('Y-m-d');
	if( isset($_POST['prices']) ){
		//print_r($_POST['prices']);die;
		foreach($_POST['prices'] as $uid=>$arr_no){
		foreach($arr_no as $tid=>$price){
			$price = intval($price);
			$sql="SELECT count(*) as n from {user_refer_course} WHERE realm='prices' AND t_uid='".$uid . "' AND course_no='".$tid . "'";
			$tmp=db_query( $sql )->fetchObject();
			if($tmp->n <= 0){
			$sql="REPLACE INTO {user_refer_course} SET realm='prices'"
				. ",s_uid=0,date='{$date}',course_end_time='',t_uid='".$uid . "',course_no='".$tid . "',course_start_time='".$price . "' ";
			}
			else{
			$sql="UPDATE {user_refer_course} SET date='{$date}',course_start_time='".$price . "' WHERE realm='prices' AND t_uid='".$uid . "' AND course_no='".$tid . "'";
			}
			db_query( $sql );//print_r($sql);
		}
		}
		drupal_set_message(t('The configuration options have been saved.'));

	}

	$default_class_points=variable_get('yaouseit_default_class_points', 300);

	$jiao_cai_course = yaouseit_get_jiao_cai_course();
		
	$arr_all_student_booking = array();
	$result = db_query("SELECT * FROM {user_refer_course} WHERE realm='prices'");
	while ($record = $result->fetchObject()) {
		$arr_all_student_booking[$record->t_uid][$record->course_no] = $record->course_start_time;
	}

//teacher_uid -> profile_id
$arr_teacher_profile_id=array();
$arr_profile_teacher_id=array();
$result = db_query("SELECT * FROM {profile} WHERE type='teacher_profile'");
while ($record = $result->fetchObject()) {
	$uid=$record->uid;
	$arr_teacher_profile_id[$uid]=$record->pid;
	//$arr_profile_teacher_id[$record->pid]=$uid;
}

$result = db_query("SELECT * FROM {users} u JOIN {users_roles} r ON u.uid=r.uid AND r.rid=" . variable_get('yaouseit_teacher_rid', 6) );
$allchkboxes = $chkboxes = '';
while ($record = $result->fetchObject()) {
	//print_r($record);die;
	
	$arr_j = array();
	$result2 = db_query("SELECT * FROM {field_data_field_jiao_cai} WHERE entity_type='profile2' AND bundle='teacher_profile' AND deleted=0 AND entity_id=".$arr_teacher_profile_id[$record->uid]);
	while ($record2 = $result2->fetchObject()) {
		$j = $record2->field_jiao_cai_tid;
		//$t_uid = $arr_profile_teacher_id[$record2->entity_id];
		$arr_j[]=$j; //86 87 91 92
	}
	$chkboxes = '';
	foreach($arr_j as $j){ //86 87 91 92
		if(!isset($jiao_cai_course[$j]))break;
		$class_points=isset($arr_all_student_booking[$record->uid][$j])?$arr_all_student_booking[$record->uid][$j]:$default_class_points;
$chkboxes .= "<li>".$jiao_cai_course[$j]['name'].": <input type=text style='width:25px;' length=5 name=prices[".$record->uid."][$j] value=$class_points></input></li>\n";
	}
	$allchkboxes .= "<div style='float:left;'><h4 style='text-align: center;'>".$record->name."</h4><ul>\n$chkboxes\n</ul></div>\n\n";
}
	$form2 = '<form action="'.$base_url.'/admin/config/yaouseit/pricing" method="post">';
	$markup = "<h3>说明：价格单位是'点数'，默认的每节课程价格是{$default_class_points}，没有设置的都用默认值。</h3><hr>".
	"\n$form2". "\n$allchkboxes\n<div style='clear:both;' /><input type=submit value='提交'></input></form>";

  $form['yaouseit_price'] = array(
    '#type'          => 'item',
    '#title'         => '上课定价',
    '#markup'          => $markup,
  );

  return $form;

}
