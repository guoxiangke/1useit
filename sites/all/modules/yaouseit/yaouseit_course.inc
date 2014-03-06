<?php

function yaouseit_get_class_prices($arr_uids) {
		$arr_prices = array();
		$sql="SELECT * FROM {user_refer_course} WHERE realm='prices' AND t_uid in (".join(',',$arr_uids) . ")";
		$result = db_query($sql);
		while ($record = $result->fetchObject()) {
			$arr_prices[$record->t_uid][$record->course_no] = $record->course_start_time;
		}//print_r($arr_prices);die;
		return $arr_prices;
}

/**
 * Page callback for our general info page.
 */
function yaouseit_course_booking() {
	//return array('#markup' => yaouseit_calendar_mycalendar());return;
	//print_r($_POST);die;
/*
Array
(
    [select] => Array
        (
            [3] => Array
                (
                    [12] => 424
                    [13] => 424
                    [14] => 424
                    [15] => 424
                    [16] => 424
                    [17] => 424
                    [18] => 424
                    [19] => 424
                    [20] => 424
                    [21] => 424
                    [22] => 424
                    [23] => 424
                    [24] => 424
                    [25] => 424
                    [26] => 424
                    [27] => 424
                    [28] => 424
                )

            [5] => Array
                (
                    [12] => 452
                    [13] => 452
                    [14] => 452
                    [15] => 452
                    [16] => 452
                    [17] => 452
                    [18] => 452
                    [19] => 452
                    [20] => 452
                    [21] => 452
                    [22] => 452
                    [23] => 452
                    [24] => 452
                    [25] => 452
                    [26] => 452
                    [27] => 452
                    [28] => 452
                )

        )
    [time_slices] => Array
        (
            [3] => Array
                (
                    [0] => 27
                )

            [5] => Array
                (
                    [0] => 28
                )



If you just need autocomplete mechanism I would suject you to use jQuery autocomplete plugin - http://docs.jquery.com/Plugins/autocomplete.

you can just output in the template something like that:

print '<input type="text" id="user-autocomplete">';
Then in javascript code

$(document).ready('function(){
  $('#user-autocomplete').autocomplete(some-ajax-url-here)
}');
you also will need to create an ajax callback page somewhere in your module:

function YOUR_MODULE_NAME_menu(){
   $items = array();
   $items['user-autocomplete-ajax-page'] = array(
     'title' => 'AJAX:get user',
     'page callback' => 'get_user'
   );
}
function get_user(){
  $sql = 'SELECT uid, name FROM {users} WHERE name LIKE ("%s")';
  $result = db_query($sql,$_GET['request']);
  $res_str = '';
  while($object = db_fetch_object($result)){
    $res_str .= $object->name.' ['.$object->uid."]\n";
  }
  print $res_str;
}



function example_form(){  

$form['Author'] = array( 
  '#type' => 'textfield', 
  '#title' => t('Example Textfield'), 
  '#autocomplete_path' => 'user/autocomplete', 
);

  return $form;
 }

require_once(drupal_get_path('module', 'node') . '/node.pages.inc');

print drupal_get_form ('example_form');



<input type="text" id="edit-field-teacher-und-1-uid" name="field_teacher[und][1][uid]" value="" size="60" maxlength="128" class="form-text form-autocomplete" />
<input type="hidden" id="edit-field-teacher-und-1-uid-autocomplete" value="http://guob.dev.camplus.hk/user_reference/autocomplete/node/class/field_teacher" disabled="disabled" class="autocomplete" />



*/
GLOBAL $user;


  global $base_url, $base_path, $base_root;
	
	$date=preg_replace('/course\/booking\//','',$_GET['q']);	//die($date);
	$date=preg_replace('/course\/booking/','',$date);	//die($date);
	if('states'==$date){drupal_goto($base_url.'/course/booking'); die;}
	
	if( isset($_POST['date']) )$date = $_POST['date'];
	if(!$date || 'states'==$date)$date = date('Y-m-d');	//course/booking/states
	$diff_time = strtotime($date)-strtotime(date('Y-m-d'));
	$diff_month = intval(date('m',strtotime($date))) - intval(date('m'));
	if(strtotime($date) === -1 || $diff_time<0 || $diff_month>1){
		return array('#markup' => "意外的错误日期！");
	}

	if (!module_exists('userpoints')) {
		$markup = "<h3>网站系统错误：没有启用userpoints模块。</h3>";
		return array('#markup' => $markup);
	}

	if( isset($_POST['booking_uid']) && $_POST['booking_uid'] ){
		$booking_uid = $_POST['booking_uid'];
	}
	else{
		if( 1==$user->uid || in_array(variable_get('administrator_role_id', 5),array_keys($user->roles)) || in_array(variable_get('yaouseit_aid_teacher_rid', 7),array_keys($user->roles)) ) {
	
			$js = <<<TT
<script type='text/javascript'>

/*
jQuery(document).ready(function(){
  jQuery('#user-autocomplete').autocomplete({
	source: '{$base_url}/mycalendar',
	minLength: 1,
	  select: function( event, ui ) {
	  	jQuery( "#booking_uid" ).val( ui.item.id );
	  }
  })
});
*/

(function ($) {

$(document).ready(function(){
  $('#user-autocomplete').autocomplete({
	source: '{$base_url}/yaouseit_user_autocomplete',
	minLength: 1,
	  select: function( event, ui ) {
	  	$( "#booking_uid" ).val( ui.item.id );
	  }
  })
});

})(jQuery);

</script>

TT;


			drupal_add_library('system', 'ui.autocomplete');
	
			$form2 = '<form action="'.$base_url.'/course/booking" method="post">'."<input type=hidden name=date  value='{$date}'>";
			$markup = "\n$form2". "\n".'<p>You can book course for others or yourself.<p>Booking user:<input type="text" lengh=20 id="user-autocomplete" name="user-autocomplete">Select while inputing user name<input type="hidden" id="booking_uid" name="booking_uid" value="">'."<br/>\n".
			$js ."\n<div align=center style='clear:both;' />"
			."<input style='text-align: center;' type=submit value='提交'></input>"."</form>\n";

			return array('#markup' => $markup);
		}
		else $booking_uid=$user->uid;
	}

	$msg_points = '';
	$user_points = userpoints_get_current_points($booking_uid);
	if($user_points<=0){
		$msg_points = "您没有足够的点数来约课，请点击<a href='/buy-points'>[这里]</a>去购买。";
		drupal_set_message($msg_points, 'warning');
		//return array('#markup' => $markup);
	}

		
	$arr_all_student_booking = array();
	$result = db_query("SELECT * FROM {user_refer_course} WHERE flag>=0 AND date='".$date . "' AND realm='student_booking'");
	while ($record = $result->fetchObject()) {
		$arr_all_student_booking[$record->t_uid][$record->course_start_time] = $record->s_uid;
	}

	//teacher_uid -> profile_id
	$arr_teacher_profile_id=array();
	$arr_profile_teacher_id=array();
	$result = db_query("SELECT * FROM {profile} WHERE type='teacher_profile'");
	while ($record = $result->fetchObject()) {
		$uid=$record->uid;
		$arr_teacher_profile_id[$uid]=$record->pid;
		//$arr_profile_teacher_id[$record->pid]=$uid;
	}//print_r($arr_teacher_profile_id);//die;

	$jiao_cai_course = yaouseit_get_jiao_cai_course();
	//print_r($jiao_cai_course);die;
/*
*/
	$schedule = yaouseit_get_course_schedule();

	$arr_course_time_slice = yaouseit_course_time_slice($schedule);
	//print_r($arr_course_time_slice);die;

	if( isset($_POST['time_slices']) && is_array($_POST['time_slices']) ){
		$arr_xuanke = $arr_uids = array();
		foreach($_POST['time_slices'] as $uid=>$arr_no){
			$arr_uids[] = $uid;
		foreach($arr_no as $no){
			$no2 = intval($no);	//-1;
			$tid = $_POST['select'][$uid][$no2];
			$arr_xuanke[$uid][$no]=$tid;
		}
		}
		//print_r($arr_xuanke);die;
		
		$arr_prices = yaouseit_get_class_prices($arr_uids);

		$course_start_time = $course_end_time = $msg_points = '';
		$all_points = 0;
		$arr_sql = array();
		foreach($arr_xuanke as $uid => $arr_no){
	
			$arr_no2 = $arr_j = array();
			$result2 = db_query("SELECT * FROM {field_data_field_jiao_cai} WHERE entity_type='profile2' AND bundle='teacher_profile' AND deleted=0 AND entity_id=".$arr_teacher_profile_id[$uid]);
			while ($record2 = $result2->fetchObject()) {
				$j = $record2->field_jiao_cai_tid;
				//$t_uid = $arr_profile_teacher_id[$record2->entity_id];
				$arr_j[]=$j; //29 32 34 35 36
			}

			foreach($arr_no as $no=>$c_tid){
				if( !isset($arr_all_student_booking[$uid][$no]) || $booking_uid==$arr_all_student_booking[$uid][$no] ){
					foreach($arr_j as $j){ //86 87 91 92
						if(!isset($jiao_cai_course[$j]))break;
						foreach($jiao_cai_course[$j]['courses'] as $jiao_cai_course_tid=> $c_name){
							if($jiao_cai_course_tid==$c_tid){
								$points=variable_get('yaouseit_default_class_points', 300);
								$points = isset($arr_prices[$uid][$j]) ? $arr_prices[$uid][$j] : $points;
								$all_points += $points;
								$msg_points .= $jiao_cai_course[$j]['name'].' 点数:'.$points."<br/>\n";
								break;
							}
						}
					}

					$sql="SELECT course_end_time,flag FROM {user_refer_course} WHERE flag<2 AND realm='student_booking' AND s_uid='".$booking_uid
					. "' AND t_uid='".$uid . "' AND date='".$date . "' AND course_start_time='".$no."'";
					$result3 = db_query($sql);
					$record3 = $result3->fetchObject();
					if( isset($record3) && !empty($record3) ){
						if( isset($record3->course_end_time) && $record3->course_end_time>0 ){
							//$arr_sql[]="UPDATE {node} set status=1 WHERE nid=".$record3->course_end_time;
							//$arr_sql[]="UPDATE {node_revision} set status=1 WHERE nid=".$record3->course_end_time;
							$nodes1[]=$record3->course_end_time;
							$arr_sql[]="UPDATE {user_refer_course} set flag=1 WHERE realm='student_booking' AND s_uid='".$booking_uid
							. "' AND t_uid='".$uid . "' AND date='".$date . "' AND course_start_time='".$no."'";
						}
					}
					else{
						/*$arr_sql[]="DELETE FROM {user_refer_course} WHERE realm='student_booking' AND s_uid='".$booking_uid
					. "' AND t_uid='".$uid . "' AND date='".$date . "' AND course_start_time=".$no;*/
						$arr_sql[]="INSERT INTO {user_refer_course} SET realm='student_booking',s_uid='".$booking_uid
					. "',t_uid='".$uid . "',date='".$date . "',course_no='".$c_tid . "',course_start_time='".$no . "',course_end_time='".$course_end_time."'" ;
					}
				}
				$arr_no2[] = "'$no'";
			}
			//cancel booking
			$arr_sql[]="UPDATE {user_refer_course} set flag=-1 WHERE realm='student_booking' AND s_uid='".$booking_uid
						. "' AND t_uid='".$uid . "' AND date='".$date . "' AND course_start_time not in (".join(',',$arr_no2) . ")";
			/*$arr_sql[]="UPDATE {node} n,{user_refer_course} r set n.status=0 WHERE n.nid=r.course_end_time AND r.realm='student_booking' AND r.s_uid='".$booking_uid
						. "' AND r.t_uid='".$uid . "' AND r.date='".$date . "' AND r.course_start_time not in (".join(',',$arr_no2) . ")";
			$arr_sql[]="UPDATE {node_revision} n,{user_refer_course} r set n.status=0 WHERE n.nid=r.course_end_time AND r.realm='student_booking' AND r.s_uid='".$booking_uid
						. "' AND r.t_uid='".$uid . "' AND r.date='".$date . "' AND r.course_start_time not in (".join(',',$arr_no2) . ")";
			*/
			$sql="SELECT course_end_time,flag FROM {user_refer_course} WHERE flag<2 AND realm='student_booking' AND s_uid='".$booking_uid
			. "' AND t_uid='".$uid . "' AND date='".$date . "' AND course_start_time not in (".join(',',$arr_no2) . ")" . " AND course_end_time>0";
			$result3 = db_query($sql);
			$record3 = $result3->fetchObject();
			if( isset($record3) && !empty($record3) ){
				$nodes0[]=$record3->course_end_time;
			}
		}
		
		drupal_set_message("您的点数:$user_points,约课点数:$all_points<br/>\n(".$msg_points.')');
		if($user_points<$all_points){
			$msg_points = "您没有足够的点数来约课，请点击<a href='/buy-points'>[这里]</a>去购买。";
			drupal_set_message($msg_points, 'warning');
		}else{
			foreach( $arr_sql as $sql ){
			 //print_r($sql);
			 db_query($sql);
			}
			if( isset($nodes0) || isset($nodes1) ){
				module_load_include('inc', 'node', 'node.admin');
				if( isset($nodes0) ){
					node_mass_update($nodes0, array('status' => 0));
				}
				if( isset($nodes1) ){
					node_mass_update($nodes1, array('status' => 1));
				}
			}
			yaouseit_create_class_nodes();
		}

	}
//die;


	//drupal_add_js(drupal_get_path('module', 'yaouseit') . '/js/jquery.js');
	drupal_add_js(drupal_get_path('module', 'yaouseit') . '/js/datepicker.js');
	//drupal_add_js(drupal_get_path('module', 'yaouseit') . '/js/eye.js');
	//drupal_add_js(drupal_get_path('module', 'yaouseit') . '/js/utils.js');
	//drupal_add_js(drupal_get_path('module', 'yaouseit') . '/js/yaouseit.js');
	drupal_add_css(drupal_get_path('module', 'yaouseit') . '/css/datepicker_nocolor.css');
	//drupal_add_css(drupal_get_path('module', 'yaouseit') . '/css/layout.css');
	//$date = date('Y').'-'.date('m').'-'.date('d');
	$current = date('Y').'-'.(date('m')+1).'-01';
	
$js = <<<TT
<script type='text/javascript'>

	var initLayout = function() {

		var now = new Date();
		now.setHours(0,0,0,0);
		jQuery('#date').DatePicker({
			flat: true,
			date: ['{$date}'],
			current: '{$current}',
			format: 'Y-m-d',
			calendars: 2,
			onRender: function(date) {
				return {
					disabled: (date.valueOf() < now.valueOf()),
					className: date.valueOf() < now.valueOf() ? 'datepickerSpecial' : false
				}
			},
			onChange: function(formated, dates) {
				window.location.replace('{$base_url}/course/booking'+'/'+formated);
			},
			starts: 1
		});

	};
	
	jQuery(document).ready(function() {	initLayout();	});

</script>

TT;

/*
SELECT *
FROM `ui_field_data_field_jiao_cai`
entity_type bundle deleted entity_id revision_id language delta field_jiao_cai_tid
taxonomy_term 	english_courses 	0 	96 	96 	und 	0 	86
taxonomy_term 	english_courses 	0 	96 	96 	und 	1 	87
*/
	$arr_course_jao_cai = array();
	$result = db_query("SELECT * FROM {field_data_field_jiao_cai} WHERE entity_type='taxonomy_term' AND bundle='english_courses' AND deleted=0");
	while ($record = $result->fetchObject()) {
		$arr_course_jao_cai[$record->entity_id][] = $record->field_jiao_cai_tid;
	}
	//print_r($arr_course_jao_cai);die;
/*
Array
(
    [96] => Array
        (
            [0] => 86
            [1] => 87
            [2] => 88
            [3] => 89
            [4] => 90
            [5] => 91
            [6] => 92
            [7] => 93
            [8] => 94
            [9] => 95
        )

)
*/
/*
SELECT *
FROM `ui_field_data_field_course`
entity_type bundle deleted entity_id revision_id language delta field_course_tid
profile2 	teacher_profile 	0 	5 	5 	und 	0 	96
profile2 	teacher_profile 	0 	5 	5 	und 	1 	97
*/

	$arr_student_booking = array();
	$result = db_query("SELECT * FROM {user_refer_course} WHERE flag>=0 AND s_uid='".$booking_uid . "' AND date='".$date . "' AND realm='student_booking'");
	while ($record = $result->fetchObject()) {
		$arr_student_booking[$record->t_uid][$record->course_start_time] = $record->course_no;
	}
//print_r($arr_student_booking);die;
/*
Array
(
    [3] => Array
        (
            [1] => 430
            [2] => 432
        )

    [5] => Array
        (
            [3] => 462
            [4] => 463
        )
*/

$arr_uids = array();
$result = db_query("SELECT u.uid FROM {users} u JOIN {users_roles} r ON u.uid=r.uid AND r.rid=" . variable_get('teacher_role_id', 6) );
while ($record = $result->fetchObject()) {
	$arr_uids[]=$record->uid;
}
$arr_prices = yaouseit_get_class_prices($arr_uids);


  module_load_include('inc', 'popup', 'includes/popup.api');

$result = db_query("SELECT u.*,r.*,b.field_bio_value,f.uri FROM {users} u JOIN {users_roles} r ON u.uid=r.uid AND r.rid=" . variable_get('teacher_role_id', 6) ." LEFT JOIN {field_data_field_bio} b ON  b.entity_type='user' AND b.bundle='user' AND u.uid=b.entity_id LEFT JOIN {field_data_field_video} v ON  v.entity_type='profile2' AND v.bundle='teacher_profile' AND u.uid=v.entity_id LEFT JOIN {file_managed}  f ON v.field_video_fid=f.fid" );
$allchkboxes = '';
	$hm = intval(date('Hi'));
	/*$h=intval(date('H')-variable_get('yaouseit_booking_no_cancel_time',5));
	$h= $h>0 ? $h : 0;
	$hm2 = intval( $h . date('i') );*/
while ($record = $result->fetchObject()) {
	//print_r($record);die;

	if( !isset($arr_teacher_profile_id[$record->uid]) )continue;
	
	$arr_j = array();
	$result2 = db_query("SELECT * FROM {field_data_field_jiao_cai} WHERE entity_type='profile2' AND bundle='teacher_profile' AND deleted=0 AND entity_id=".$arr_teacher_profile_id[$record->uid]);
	while ($record2 = $result2->fetchObject()) {
		$j = $record2->field_jiao_cai_tid;
		//$t_uid = $arr_profile_teacher_id[$record2->entity_id];
		$arr_j[]=$j; //86 87 91 92
	}

	
	$arr_unavailable = array();
	$sql = "SELECT u.field_unavailable_time_tid,t.name FROM {field_data_field_unavailable_time} u JOIN {taxonomy_term_data} t ON u.field_unavailable_time_tid=t.tid WHERE entity_type='profile2' AND bundle='teacher_profile' AND deleted=0 AND entity_id=".$arr_teacher_profile_id[$record->uid];
	$result2 = db_query($sql);//print_r($sql);
	while ($record2 = $result2->fetchObject()) {
		$tid = $record2->field_unavailable_time_tid;
		if( isset($arr_course_time_slice['parent'][$tid]) ){
			foreach($arr_course_time_slice['parent'][$tid] as $child){
				$arr_unavailable[]=$child;
			}
		}
		else $arr_unavailable[]=$tid;
		
	}//print_r($arr_unavailable);
	
	$arr_unavailable1 = array();
	$day = date('w')+1;//var_dump($day);//die($day);
	//if( isset($arr_course_time_slice['daytime'][$day]) ){
			foreach($arr_course_time_slice['daytime'][$day] as $time=>$tid){//var_dump($time,$tid);
				if( in_array($tid,$arr_unavailable) ){
				$arr_unavailable1[]=$time;
				}
			}
	//}
	//print_r($arr_unavailable1);

	$chkboxes = '';
	foreach($schedule as $no => $one){	//1 2 ... 28
		$h5=intval( preg_replace('/\:.*/','',$one[0])-variable_get('yaouseit_booking_no_cancel_time',5));
		$h5= $h5>0 ? $h5 : 0;
		$h5 = intval( $h5 . preg_replace('/.*\:/','',$one[0]) );//var_dump($h5);

		if( 0 || $date!=date('Y-m-d') || intval(preg_replace('/[^\d]/','',$one[0])) > $hm ){
			$checked0=$checked='';
			if( isset($arr_student_booking[$record->uid]) ){
			foreach($arr_student_booking[$record->uid] as $schedule_no=> $nouseid){
				if($no==$schedule_no){
					$checked0 ="checked=checked";
					break;
				}
			}
			}
			
			if( isset($arr_unavailable1) ){
			foreach($arr_unavailable1 as $start){
				if($start == $one[0]){
					$checked ="disabled";
					break;
				}
			}
			}

			if( 1 && $date==date('Y-m-d') && $h5 < $hm ){
				foreach($arr_j as $j){ //86 87 91 92
					if(!isset($jiao_cai_course[$j]))break;
					foreach($jiao_cai_course[$j]['courses'] as $c_tid=> $c_name){
						$selected='';
							if( isset($arr_student_booking[$record->uid]) ){
							foreach($arr_student_booking[$record->uid] as $schedule_no=> $tid){	//t_uid=3:1=>430 2=>432 //t_uid=5:3=>462 4=>263
								if($no==$schedule_no && $c_tid==$tid){
									$checked ="disabled";
									break;
								}
							}
							}
					}
				}
			}
			if( isset($arr_all_student_booking[$record->uid]) ){
			foreach($arr_all_student_booking[$record->uid] as $schedule_no=> $s_uid){
				if($no==$schedule_no && $booking_uid != $s_uid){
					$checked ="disabled";
					break;
				}
			}
			}

			$select = $options = '';
			if( 1 || $checked !="disabled" ){
				foreach($arr_j as $j){ //86 87 91 92
					if(!isset($jiao_cai_course[$j]))break;
					
					$points=variable_get('yaouseit_default_class_points', 300);
					$points = isset($arr_prices[$record->uid][$j]) ? $arr_prices[$record->uid][$j] : $points;
					$options .= "<optgroup label=\"".$jiao_cai_course[$j]['name']." 点数:$points\">\n";
					foreach($jiao_cai_course[$j]['courses'] as $c_tid=> $c_name){
						$selected='';
							if( isset($arr_student_booking[$record->uid]) ){
							foreach($arr_student_booking[$record->uid] as $schedule_no=> $tid){	//t_uid=3:1=>430 2=>432 //t_uid=5:3=>462 4=>263
								if($no==$schedule_no && $c_tid==$tid){
									$selected ="selected";
									break;
								}
							}
							}
						$options .= "<option value=$c_tid $selected>$c_name</option>\n";
					}
					$options .= "</optgroup>\n";

				}
			}
			
			$select = "<select name='select[".$record->uid."][$no]'>\n".$options."\n</select>\n";

			$chkboxes .= "<li><input type=checkbox $checked0 $checked name=time_slices[".$record->uid."][$no] value=$no></input>".$one[0] .' - '.$one[1].$select."</li>\n";
		}
	}
	
	
	$picture='';
	if($record->picture){
		$res_pic = db_query("SELECT uri FROM {file_managed} WHERE fid='" .$record->picture."'" );
		$record_pic = $res_pic->fetchObject();
		//file_create_url($record_pic->uri)
		$picture='<img style="padding:0 5px 0 30px;" align=left src="'.image_style_url('thumbnail', $record_pic->uri). '">';
	}
	else $picture='<img style="padding:0 5px 0 30px;" align=left src="'.image_style_url('thumbnail',"public://default_profile_1.png"). '">';

	$video='';
	if(module_exists('mediaelement')) {
	if($record->uri){
    drupal_add_library('mediaelement', 'mediaelement');
    drupal_add_js(drupal_get_path('module', 'mediaelement') . '/mediaelement.js');

	$variables['attributes'] = Array( 'src'=>file_create_url($record->uri), 'class'=>'mediaelement-formatter-identifier-'.$record->uid, 'controls' => 'controls','height' => 385,'width' => 640);
	$variables['settings'] = Array( 'controls' => 1,'height' => 385,'width' => 640,'download_link'=>'','download_text' => 'Download');
    $video=theme_mediaelement_video($variables);
    }
    }
    $popup=$record->name;
	if(module_exists('mediaelement')) {
	  $popup = popup(array('text' => l($record->name, "user/$record->uid").$video.$record->field_bio_value,'title'=>$record->name, 'activate' => 'hover','width'=>'660', 'close' => TRUE));
	}
	
	$allchkboxes .= "<div style='float:left;'>".$picture."<br /><br />".$popup."<br /><br /><br /><ul>\n$chkboxes\n</ul></div>\n\n";	//<h4 style='text-align: center;'></h4>
}

	if( isset($_POST['user-autocomplete']) ) $booking_username=$_POST['user-autocomplete'];
	else $booking_username=$user->name;
	$form2 = '<form action="'.$base_url.'/course/booking" method="post">'."<input type=hidden name=date  value='{$date}'>" .'<input type="hidden" id="booking_uid" name="booking_uid" value="'.$booking_uid.'">' .'<input type="hidden" id="user-autocomplete" name="user-autocomplete" value="'.$booking_username.'">';
	$markup = "<h4 align=center>已选择的约课用户：".$booking_username."</h4>\n<h4 align=center>已选择的约课日期：$date</h4><p align=center id='date'></p>\n" . $js ."\n$form2". "\n$allchkboxes\n<div align=center style='clear:both;' />"
	.($user->uid<=0 || $user_points<=0 ? '':"<input style='text-align: center;' type=submit value='提交'></input>")."</form>\n";

	$msg = "上课日期、时间、老师、内容由学生自主选择，请按您要选择的老师在上课时间左边打钩并选择上课内容，根据具体的选择费用可能有不同。";
	drupal_set_message($msg);
	$msg = "约课不扣除点数，在每节课程上课时间开始前".variable_get('yaouseit_booking_no_cancel_time', 5)."小时才扣除相应的点数，并且不再允许您取消约课，在每节课程上课时间开始".variable_get('yaouseit_booking_no_cancel_time', 5)."小时前允许您取消约课。";
	$msg = "您拥有的点数是{$user_points}，这里您可以约多次课，但是请留意您有足够的点数来约课，不然约课不能成功。";
	drupal_set_message($msg);
	$msg = "约课不扣除点数，在每节课程上课时间开始前".variable_get('yaouseit_booking_no_cancel_time', 5)."小时才扣除相应的点数，并且不再允许您取消约课，在每节课程上课时间开始".variable_get('yaouseit_booking_no_cancel_time', 5)."小时前允许您取消约课。";
	drupal_set_message($msg, 'warning');
	return array('#markup' => $markup);

}


function yaouseit_create_class_nodes() {
	global $user;

	$date=date('Y-m-d');

	$schedule = yaouseit_get_course_schedule();
	$where1 = 'flag=0 AND ';
	$sql="SELECT * FROM {user_refer_course} r WHERE $where1 realm='student_booking' ORDER BY r.`date`,r.course_start_time,r.t_uid,r.s_uid";

	$arr_class =$arr_uids= array();
	//die($sql);
	$result = db_query($sql);
	while ($record = $result->fetchObject()) {
		$arr_class[$record->date][$record->course_start_time][$record->t_uid]['s_uids'][] = $record->s_uid;
		$arr_class[$record->date][$record->course_start_time][$record->t_uid]['course_no'] = $record->course_no;
		$arr_uids[$record->t_uid]=$record->t_uid;
		$arr_s_uids[$record->s_uid]=$record->s_uid;
		//$arr_course_no[]=$record->course_no;
	}
	if( empty($arr_class) )return;
	
	$arr_uids = array_keys($arr_uids);
	//print_r($arr_class);print_r($arr_uids);die;
	$arr_s_uids = array_keys($arr_s_uids);

	$arr_user = array();
	$sql="SELECT * FROM {users} WHERE uid in (".join(',',$arr_uids) . ")";
	//die($sql."<br>\n");
	$result = db_query($sql);
	while ($record = $result->fetchObject()) {
		$arr_user[$record->uid]=$record;
	}

	$arr_s_user = array();
	$sql="SELECT * FROM {users} WHERE uid in (".join(',',$arr_s_uids) . ")";
	//die($sql."<br>\n");
	$result = db_query($sql);
	while ($record = $result->fetchObject()) {
		$arr_s_user[$record->uid]=$record;
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
		$class_node->comment=2;

		$students = '';
		foreach($arr_t['s_uids'] as $s_uid){
			$students .= $arr_s_user[$s_uid]->name.' ';
		}

		foreach($jiao_cai_course as $j => $arr){ //86 87 91 92
			foreach($arr['courses'] as $c_tid=> $c_name){
				if($c_tid==$arr_t['course_no']){
					$class_node->title = $students .t('has booked').' '.$arr_user[$t_uid]->name.t("'s").' '.t('class').":$adate ".$schedule[$no][0].' '. $arr['name'].' '.$c_name;
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
		$class_nid = $class_node->nid;
		//ALTER TABLE  `user_refer_course` ADD  `flag` INT NOT NULL DEFAULT  '0'
		$sql="UPDATE {user_refer_course} SET flag=1,course_end_time=$class_nid WHERE realm='student_booking' "
				. " AND t_uid='".$t_uid . "' AND date='".$adate . "' AND course_start_time='".$no . "' AND course_no='".$arr_t['course_no']."'" ;
		//var_dump($sql);
		$result = db_query($sql);
		
		}
	}
	}
	}
}

function yaouseit_show_teacher($uid){

	$account = user_load($uid);
	//print_r($account);die;
	$picture = $account->picture;
	$preset['presetname'] = 'thumbnail';
	$alt = isset($account->picture->filename)?$account->picture->filename:'';
	
        if (1 || !empty($account->uid) && user_access('access user profiles')) {
          $attributes = array('attributes' => array('title' => 'Profile', 'class' => 'bt_' . $account->uid ), 'html' => TRUE);
          $image = theme('imagecache', $preset['presetname'], $picture, $alt);
          $vars['picture'] = l($image, "user/$account->uid", $attributes);

          /* USER POPUP INFO FACEBOOK LIKE */

          // Load profiles fields
          profile_load_profile($account);
          
          $output =  '<div id="user-profile-details">' . "\n";
          $output .= '<div>' . $vars['picture'] . '</div>' . "\n";
          $output .= '<div>Profile Name: ' . check_plain($account->profile_nome) . '</div>' . "\n";
          $output .= '<div>realname: ' . check_plain($account->realname) . '</div>' . "\n";
          if(isset($account->field_bio['und'][0]['value']))$output .= '<div>BIO: ' . check_plain($account->field_bio['und'][0]['value']) . '</div>' . "\n";
         $output .= '</div>' . "\n";

          // Beautytips code
          if(module_exists('beautytips')) {
            /* UNIQUE NAME */
            $bt_name_id = 'bt_user_picture_' . $account->uid;
            /* UNIQUE SELECTOR */
            $cssSelect = 'div.picture .bt_' . $account->uid;
            $options[$bt_name_id] = array(
              'cssSelect' => $cssSelect,
              'text' => $output,
              'hoverIntentOpts' => array(
                'interval' => 100,
                'timeout' => 2000,
              ),
            );
            beautytips_add_beautytips($options);
            drupal_add_js(drupal_get_path('module', 'beautytips'). '/other_libs/jquery.hoverIntent.minified.js');
          }
          
          /* END - USER POPUP INFO FACEBOOK LIKE */

        }
        else {
          $attributes = array('class' => 'tooltipsy');
          $vars['picture'] = theme('imagecache', $preset['presetname'], $picture, $alt, $title, $attributes);
        }
}

function yaouseit_user_autocomplete(){//print_r($_GET);die;
  $string = $_GET['term'];
  if ($string) {
    $result = db_select('users')->fields('users', array('uid','name'))->condition('name', db_like($string) . '%', 'LIKE')->range(0, 10)->execute();
    foreach ($result as $user) {
      //$matches[$user->name] = check_plain($user->name);
      $matches[] = array( 'id'=>$user->uid,'value'=>check_plain($user->name) );
    }
  }

  drupal_json_output($matches);
}

function yaouseit_recommend_students($type=null){
	global $user;

/*
db_query("SELECT tu.*, t1.name as student_level_name, t2.name as school_name, t3.name as study_goal_name
 FROM tmp_users tu
 LEFT JOIN taxonomy_term_data t1 ON tu.student_level=t1.tid
 LEFT JOIN taxonomy_term_data t2 ON tu.school=t2.tid
 LEFT JOIN taxonomy_term_data t3 ON tu.study_goal=t3.tid
 WHERE tu.student_level>0 ORDER BY "
 .( $user->uid>0 ? "IF(student_level={$student_level},0,1),IF(school={$school},0,1),IF(study_goal={$study_goal},0,1)," : "")
 "student_level,school,study_goal,gender";

$query = db_select('node', 'n')->extend('TableSort');
$query
  ->orderBy('uid', 'DESC') // Sort by uid first.
  ->orderByHeader($header) // Then by what has been selected in the table header
  ->orderBy('changed', 'DESC') // And at last, by the changed date.

$node_rs = db_select('node', 'n');
    $node_rs -> join('field_data_field_image', 'i','n.nid = i.entity_id');
    $node_rs -> join('file_managed', 'f','f.fid = i.field_image_fid');
    $node_rs -> join('field_data_body', 'b','b.entity_id = n.nid');        
    $node_rs -> fields('n')
             -> fields('i')
             -> fields('f')
             -> fields('b')                  
             -> condition('type', 'article')
             -> condition('field_browser_by_artist_nid', array(1,5,7), 'IN')
             -> condition('title', '%' . db_like($search_news) . '%', 'LIKE');            
    $result = $node_rs
             -> extend('PagerDefault')
             -> limit(5)
             -> orderBy('title', $_SESSION['sort_title'])
             -> orderBy('created', $_SESSION['sort_time'])
             -> execute();
*/
$student_level = 14;
$school = 92;
$study_goal = 4051;
$query = db_select('tmp_users', 'tu')->extend('PagerDefault');
$query->leftJoin('taxonomy_term_data', 't1','tu.student_level=t1.tid');
$query->leftJoin('taxonomy_term_data', 't2','tu.school=t2.tid');
$query->leftJoin('taxonomy_term_data', 't3','tu.study_goal=t3.tid');
$query->fields('tu');
$query->addField('t1', 'name', 'student_level_name');
$query->addField('t2','name', 'school_name');
$query->addField('t3','name', 'study_goal_name');
if( 'block'!=$type ) {
	$query->limit(2);
}
else{
	$query->limit(4);
}

//$query->extend('TableSort');
if( $user->uid>0 ){
$query->orderBy("IF(student_level={$student_level},0,1)", 'ASC');
$query->orderBy("IF(school={$school},0,1)", 'ASC');
$query->orderBy("IF(study_goal={$study_goal},0,1)", 'ASC');
}
$query->orderBy('student_level', 'ASC');
$query->orderBy('school', 'ASC');
$query->orderBy('study_goal', 'ASC');
$query->orderBy('gender', 'ASC');
$result = $query->execute();
//print_r($result->fetchAll());die;
  $rows = array();

  $header = array(
    '', // Icon column.
    array('data' => t('Username'), 'field' => 'tu.username'),
    array('data' => t('Student level'), 'field' => 't1.student_level_name'),
    array('data' => t('School'), 'field' => 't2.school_name'),
    array('data' => t('Study goal'), 'field' => 't3.study_goal_name'),
    array('data' => t('Operations')),
  );
  if( 'block'!=$type ) {
	  $header = array(
		t('Username'),
		t('Student level'),
		t('School'),
		t('Study goal'),
		t('Operations'),
	  );
  }
  else{
	  $header = array(
		t('Username'),
		t('Student level'),
		t('Operations'),
	  );
  }

  foreach ($result as $dblog) {
  if( 'block'!=$type ) {
    $rows[] = array('data' =>
      array(
        theme('username', array('account' => $dblog)),
        $dblog->student_level_name,
        $dblog->school_name,
        $dblog->study_goal_name,
        flag_create_link('commons_follow_user', $dblog->uid),
      ),
      // Attributes for tr
      //'class' => array(drupal_html_class('dblog-' . $dblog->type), $classes[$dblog->severity]),
    );
  }
  else{
    $rows[] = array('data' =>
      array(
        theme('username', array('account' => $dblog)),
        $dblog->student_level_name,
        flag_create_link('commons_follow_user', $dblog->uid)
      ),
      // Attributes for tr
      //'class' => array(drupal_html_class('dblog-' . $dblog->type), $classes[$dblog->severity]),
    );
  }
  }

  $build['students_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#attributes' => array('id' => 'tuijian-students'),
    '#empty' => t('No records available.'),
  );
  if( 'block'!=$type ) 
  	$build['students_pager'] = array('#theme' => 'pager');

  return $build;
}

function yaouseit_recommend_videos(){
	global $user;

	return yaouseit_recommend_students();
}

function yaouseit_mycalendar(){	//return yaouseit_tmp_table();return;//yaouseit_user_autocomplete();die;
	return yaouseit_calendar_show('page');
}

function yaouseit_calendar_show($type){
	global $user;
	if(!$user->uid)return '';
	
	if('page'==$type)$width = '100%';
	if('block'==$type)$width = '237px';


	$jiao_cai_course = yaouseit_get_jiao_cai_course();
	//print_r($jiao_cai_course);die;

	$schedule = yaouseit_get_course_schedule();

	$orig_schedule = taxonomy_get_tree(variable_get('yaouseit_course_schedule_vid', 7), $parent = 0, $max_depth = NULL, $load_entities = FALSE) ;

	$events_js = "";

		$events =$events0 = array();
		$block_events = $block_events2 = $events_b = array();
		$today = time();

		$result = db_query("SELECT * FROM {user_refer_course} r WHERE flag>=0 AND (r.s_uid=" . $user->uid . " OR r.t_uid=" . $user->uid . ") AND realm='student_booking' ORDER BY r.`date`,r.course_start_time");
		$arr_class =$arr_uids= array();
		while ($record = $result->fetchObject()) {
			$arr_class[$record->date][$record->course_start_time][$record->t_uid]['s_uids'][] = $record->s_uid;
			$arr_class[$record->date][$record->course_start_time][$record->t_uid]['course_no'] = $record->course_no;
			$arr_uids[$record->t_uid]=$record->t_uid;
			$arr_s_uids[$record->s_uid]=$record->s_uid;
			//$arr_course_no[]=$record->course_no;
		}
		if( empty($arr_class) )return;
	
		$arr_uids = array_keys($arr_uids);
		//print_r($arr_class);print_r($arr_uids);die;
		$arr_s_uids = array_keys($arr_s_uids);

		$arr_user = array();
		$sql="SELECT * FROM {users} WHERE uid in (".join(',',$arr_uids) . ")";
		//die($sql."<br>\n");
		$result = db_query($sql);
		while ($record = $result->fetchObject()) {
			$arr_user[$record->uid]=$record;
		}

		$arr_s_user = array();
		$sql="SELECT * FROM {users} WHERE uid in (".join(',',$arr_s_uids) . ")";
		//die($sql."<br>\n");
		$result = db_query($sql);
		while ($record = $result->fetchObject()) {
			$arr_s_user[$record->uid]=$record;
		}
		
		$result = db_query("SELECT * FROM {user_refer_course} r WHERE flag>=0 AND (r.s_uid=" . $user->uid . " OR r.t_uid=" . $user->uid . ") AND realm='student_booking' ORDER BY r.`date`,r.course_start_time");
		while ($record = $result->fetchObject()) {//print_r($record);die;

			$students = '';
			/*foreach($arr_t['s_uids'] as $s_uid){
				$students .= $arr_s_user[$s_uid]->name.' ';
			}*/
			$students = $arr_s_user[$record->s_uid]->name.' ';

			$title = $block_title = $class_time = '';
			$no = $record->course_start_time;
			foreach($jiao_cai_course as $j => $arr){ //86 87 91 92
				foreach($arr['courses'] as $c_tid=> $c_name){
					if($c_tid==$record->course_no){
						$title = $students .t('has booked').' '.$arr_user[$record->t_uid]->name.t("'s").' '.t('class').":$record->date ".$schedule[$no][0].' '. $arr['name'].' '.$c_name;
						$class_time=' '.$schedule[$no][0];
						break;
					}
				}
			}

			$startDate = strtotime($record->date.$class_time);
			$diffdays=abs( 365*( intval(date('Y',$startDate))-intval(date('Y',$today)) ) + intval(date('z',$startDate))-intval(date('z',$today)));
			$events[] = "{
				title: '".my_html4js($title.' '.$record->course_start_time)."',
				start: new Date(y,m,d" . ($startDate>$today?"+$diffdays":"-$diffdays") . "," . date('H',$startDate) . "," . date('i',$startDate) . ")," . 
				"	description: ''," .
				"	url: '/node/".$record->course_end_time."', allDay: false
				}";
				$key = ($startDate>$today?"+$diffdays":"-$diffdays");
				if(!isset($block_events[$key]))$block_events[$key]=array();
				array_push($block_events[$key], my_html4js('<a target=_blank  href="' . '/node/'.$record->course_end_time .'">' . $title . '</a>'));
				if(!isset($block_events2[$key]))$block_events2[$key]=array();
				$block_events2[$key]['Y']=date('Y',$startDate);
				$block_events2[$key]['m']=date('m',$startDate);
				$block_events2[$key]['d']=date('d',$startDate);
		}
		
		ksort($block_events);reset ($block_events);
		//print_r($block_events);die;//print_r($events);
		$no = 1;
		foreach($block_events as $key=>$arr1){
			$events_b[] = "{
				title: 'No.".$no.'('.count($arr1).")',
				start: new Date(y,m,d" . "{$key}" . "),"
				. 
				"	description: '" . join('<br/>',$arr1) . "'," .
				"	url: '/calendar/view.php?view=day&cal_d={$block_events2[$key]['d']}&cal_m={$block_events2[$key]['m']}&cal_y={$block_events2[$key]['Y']}', allDay: false
				}";
				$no++;
		}
		//if(!isset($events)){db_set_active();return '';}
		if('page'==$type)$events_js = "events: [\n" . join(',',$events) . "],\n";
		if('block'==$type)$events_js = "events: [\n" . join(',',$events_b) . "],\n";
		//if('block'==$type)$events_js = "events: [\n" . join(',',$events) . "],\n";
	
	drupal_add_css('sites/all/libraries/fullcalendar/fullcalendar.css');
	drupal_add_js('sites/all/libraries/fullcalendar/fullcalendar.min.js');
	drupal_add_js('sites/all/libraries/qtip/jquery.qtip-1.0.0-rc3.min.js');

	$s = <<<TT
<script type='text/javascript'>

	jQuery(document).ready(function() {
	
		var date = new Date();
		var d = date.getDate();
		var m = date.getMonth();
		var y = date.getFullYear();
		
		jQuery('#mycalendar_{$type}').fullCalendar({
			header: {
				left: 'today, prev, next',
				center: 'title',
				right: 'month,agendaWeek,agendaDay'
			},
			editable: false,
			$events_js
        viewDisplay: function(view) { 
         var calendar = jQuery(this);
         jQuery('.fc-event').each(function(){
            jQuery(this).attr('target','_blank');
            // Grab event data
            var title = jQuery(this).find('.fc-event-title').text(),
               data = calendar.data('fullCalendar').clientEvents(function(event){
                  return event.title === title;
               })[0];
            var qtipContent = data.description ? data.description : data.title;
            jQuery(this).qtip({
               content: qtipContent,
               position: {
                  corner: {
                     target: 'topLeft',
                     tooltip: 'bottomLeft'
                  }
               },
               show: 'mouseover',
               hide:  { fixed: true, delay: 1000 },
               style: {
                 width: 200,
                 padding: 5,
                 background: '#A2D959',
                 color: 'black',
                 textAlign: 'center',
                 border: {
                    width: 7,
                    radius: 5,
                    color: '#A2D959'
                 },
                 tip: true
               }
            });
         });
         return false;
        }


		});
		
	});

</script>
<div id='mycalendar_{$type}' _style="width: $width;margin: 0 auto;"></div>

TT;


		$css2 = <<<TT
#mycalendar_block h2, #mycalendar_page h2 {
 font-size:16px;
}
TT;

		$s2 = <<<TT
<script type='text/javascript'>

	jQuery(document).ready(function() {
	
		var date = new Date();
		var d = date.getDate();
		var m = date.getMonth();
		var y = date.getFullYear();
		
		jQuery('#mycalendar_{$type}').fullCalendar({
			header: {
				left: 'prev',
				center: 'title',
				right: 'next'
			},
			editable: false,
			$events_js
        viewDisplay: function(view) { 
         var calendar = jQuery(this);
         jQuery('.fc-event').each(function(){
            jQuery(this).attr('target','_blank');
            // Grab event data
            var title = jQuery(this).find('.fc-event-title').text(),
               data = calendar.data('fullCalendar').clientEvents(function(event){
                  return event.title === title;
               })[0];
            var qtipContent = data.description ? data.description : data.title;
            jQuery(this).qtip({
               content: qtipContent,
               position: {
                  corner: {
                     target: 'topLeft',
                     tooltip: 'bottomLeft'
                  }
               },
               show: 'mouseover',
               hide: { fixed: true, delay: 1000 },
               style: {
                 width: 200,
                 padding: 5,
                 background: '#A2D959',
                 color: 'black',
                 textAlign: 'center',
                 border: {
                    width: 7,
                    radius: 5,
                    color: '#A2D959'
                 },
                 tip: true
               }
            });
         });
         return false;
        }


		});

jQuery("#mycalendar_{$type} > div.fc-content").eq(1).css("display","none");
jQuery("#mycalendar_{$type} > table.fc-header").eq(1).css("display","none");
jQuery("#mycalendar_{$type} .fc-event-time").text("");
		
	});

</script>
<div id='mycalendar_{$type}' _style="width: $width;margin: 0 auto;"></div>
<div align="center" class="view-full">
	<a href="/mycalendar">View Full Calendar</a>
</div>
TT;

      $block['subject'] = t('My Calendar');
      $block['content'] = $s2;


	if('page'==$type)return $s;
	if('block'==$type){
		drupal_add_css($css2,$option['type'] = 'inline');
		return $block;
	}
}

function my_html4js($s){
	return str_replace(array("'",'"','&',"\\\\","\\n","\\r","\\t","\\b","\\f","\n","\r","\t","\b","\f"), array("\\'",'\\"',"\\&",'\\\\',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '),$s);
	//return str_replace(array("'",'"','&',"\\\\","\\n","\\r","\\t","\\b","\\f"), array("\\'",'\\"',"\\&",'\\\\',' ',' ',' ',' ',' '),$s);
}
