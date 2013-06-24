<?php
// 'version'       => '1.0.0',
// 'charset'       => 'UTF-8', //UTF-8, GBK等
// 'merId'         => '105550149170027', //商户填写
// 'acqCode'       => '',  //收单机构填写
// 'merCode'       => '',  //收单机构填写
// 'merAbbr'       => '商户名称',
function uuke_unionpay_configue_form($form, &$form_state) {
  $form['uuke_unionpay_merId'] = array(
    '#type' => 'textfield',
    '#title' => t('商户代码'),
    '#description' => t('merId.'),
    '#required' => TRUE,
    '#default_value' => variable_get('uuke_unionpay_merId', '105550149170027'),
    '#size' => 16,
  );  
  $form['uuke_unionpay_security_key'] = array(
    '#type' => 'textfield',
    '#title' => t('商户密钥'),
    '#required' => TRUE,
    '#default_value' => variable_get('uuke_unionpay_security_key', '88888888'),
    '#description' => 'Q：银联Upop开发测试环境支付的测试卡号、密码？
A：借记卡卡号：6212341111111111111，密码：111111，手机号：13888888888，短信验证码测试环境不校验，可随便输6位数字。
    信用卡卡号：6212341111111111111，年份：12，月份：12，CVN2：123，短信验证码',
    '#size' => 16,
  );
  $form['uuke_unionpay_acqCode'] = array(
    '#type' => 'textfield',
    '#title' => t('uuke_unionpay_acqCode.'),
    '#description' => t('商户参数.收单机构填写'),
    '#default_value' => variable_get('uuke_unionpay_acqCode', ''),
  );
  $form['uuke_unionpay_merCode'] = array(
    '#type' => 'textfield',
    '#title' => t('uuke_unionpay_merCode.'),
    '#description' => t('商户参数.收单机构填写'),
    '#default_value' => variable_get('uuke_unionpay_merCode', ''),
  );
  $form['uuke_unionpay_merAbbr'] = array(
    '#type' => 'textfield',
    '#title' => t('商户名称.'),
    '#default_value' => variable_get('uuke_unionpay_merAbbr', ''),
  );
	//交易类型，CONSUME or PRE_AUTH
  $form['uuke_unionpay_transType'] = array(
    '#title' => t('交易种类'),
    '#type' => 'radios',
    '#default_value' => variable_get('uuke_unionpay_transType', 'CONSUME'),
    '#options' => array(
      'CONSUME' => t('直接消费'),
      'PRE_AUTH' => t('担保消费'),
    )
  );
  $form['uuke_unionpay_environment'] = array(
    '#title' => t('运行模式'),
    '#type' => 'radios',
    '#default_value' => variable_get('uuke_unionpay_environment', '1'),
    '#options' => array(
      '1' => t('测试环境'),
      '2' => t('预上线环境'),
      '3' => t('线上环境'),
    )
  );
  return system_settings_form($form);
}
function uuke_unionpay_notify(){

  module_load_include('inc', 'uuke_unionpay', 'quickpay_conf');
  module_load_include('inc', 'uuke_unionpay', 'quickpay_service');
	try {
	    $response = new quickpay_service($_POST, quickpay_conf::RESPONSE);
	    if ($response->get('respCode') != quickpay_service::RESP_SUCCESS) {
	        $err = sprintf("Error: %d => %s", $response->get('respCode'), $response->get('respMsg'));
	        throw new Exception($err);
	    }

	    $arr_ret = $response->get_args();

	    //更新数据库，将交易状态设置为已付款
	    //注意保存qid，以便调用后台接口进行退货/消费撤销
	    watchdog('uuke_unionpay_notify:更新数据库，将交易状态设置为已付款', '<pre>'.print_r($arr_ret,TRUE), array(), WATCHDOG_NOTICE, 'link');
	    //以下仅用于测试
	    // file_put_contents('notify.txt', var_export($arr_ret, true));
      ///begin
      $request = $arr_ret;
      $node = node_load($arr_ret['orderNumber']-0); //订单节点更新 000000036
      // 201304181451005901392 qid 银联交易唯一订单号
      $out_trade_no = $request['qid'];
      $node ->field_uuke_alipay_trade_no[LANGUAGE_NONE][0]['value'] = $out_trade_no;
      
      $old_status = $node->field_uuke_alipay_trade_status[LANGUAGE_NONE][0]['value']; //pending
      $new_status = $request['respMsg'];//支付成功
      if($old_status != $new_status) {//uuke_alipay_trade_status_to_chinese($request['trade_status'])
        $node->field_uuke_alipay_trade_status[LANGUAGE_NONE][0]['value'] = 'TRADE_SUCCESS';//支付成功 uuke_alipay_trade_status_to_chinese($request['trade_status'])
        watchdog('uuke_alipay_notify',$out_trade_no.'交易状态改变 '.$old_status.'=>'.$new_status);
      }
      $node->field_uuke_alipay_return[LANGUAGE_NONE][] = array(
        'value' => serialize($request+$arr_ret),//$_GET
      );
      node_save($node);
      drupal_set_message(t("订单 {$arr_ret['orderNumber']} 支付成功,请积极联系卖家以便完成交易。"), 'status', FALSE);
      drupal_goto("node/$node->nid");
      ///end

	}
	catch(Exception $exp) {
      $str .= var_export($exp, true);
      //后台通知出错
      watchdog('uuke_unionpay_notify:后台通知出错', '<pre>'.$str, array(), WATCHDOG_NOTICE, 'link');
      // file_put_contents('notify.txt', var_export($exp, true));
      drupal_set_message(t('返回数据错误13041801，请与商家联系!').$str, 'status', FALSE);
      drupal_goto("node/$node->nid");

	}
	echo 'sucess';
}
function uuke_unionpay_return() {
  module_load_include('inc', 'uuke_unionpay', 'quickpay_service');
	try {
	    $response = new quickpay_service($_POST, quickpay_conf::RESPONSE);
	    if ($response->get('respCode') != quickpay_service::RESP_SUCCESS) {
	        $err = sprintf("Error: %d => %s", $response->get('respCode'), $response->get('respMsg'));
	        throw new Exception($err);
	    }
	    $arr_ret = $response->get_args();
      $request = $arr_ret;
	    // dpm($_POST,'$_POST');
	    // dpm($arr_ret,'$arr_ret');
	    //告诉用户交易完成
      $node = node_load($arr_ret['orderNumber']-0); //订单节点更新 000000036
      //如果已经查询过订单，返回时不用再更新了
      $product_nid = $node->field_uuke_alipay_product_nid[LANGUAGE_NONE][0]['target_id'];
      if($node->field_uuke_alipay_trade_status[LANGUAGE_NONE][0]['value'] == '支付成功' &&  $node->title == $arr_ret['orderNumber'] + $node->created)  {
        drupal_goto("node/$product_nid");
      }
      // 201304181451005901392 qid 银联交易唯一订单号
      $out_trade_no = $request['qid'];

      $node ->field_uuke_alipay_trade_no[LANGUAGE_NONE][0]['value'] = $out_trade_no;
      
      // $old_status = $node->field_uuke_alipay_trade_status[LANGUAGE_NONE][0]['value']; //pending
      // $new_status = $request['respMsg'];//支付成功
      // if($old_status != $new_status) {//uuke_alipay_trade_status_to_chinese($request['trade_status'])
      //   $node->field_uuke_alipay_trade_status[LANGUAGE_NONE][0]['value'] = 'TRADE_SUCCESS';//支付成功 uuke_alipay_trade_status_to_chinese($request['trade_status'])
      //   watchdog('uuke_alipay_notify',$out_trade_no.'交易状态改变 '.$old_status.'=>'.$new_status);
      // }

      $node->field_uuke_alipay_trade_status[LANGUAGE_NONE][0]['value'] = $request['respMsg'];//'TRADE_SUCCESS';//支付成功 

      $node->field_uuke_alipay_return[LANGUAGE_NONE][] = array(
        'value' => serialize($request+$arr_ret),//$_GET
      );

      $node->title = $arr_ret['orderNumber'] + $node->created;
      
      $order->field_uuke_pay_method[LANGUAGE_NONE][0]['value'] = '2'; //unionpay 
      // $order ->title = $out_trade_no + $order ->created;
      node_save($node);
      drupal_set_message(t("订单 {$node ->title} 支付成功,请积极联系卖家以便完成交易。"), 'status', FALSE);
      drupal_goto("node/$product_nid");

	}
	catch(Exception $exp) {
	    $str = var_export($exp, true);
      watchdog('uuke_unionpay error', $str, array(), WATCHDOG_NOTICE, 'link');
      drupal_set_message(t('返回数据错误13041801，请与商家联系!').$str, 'status', FALSE);
      drupal_goto("node/$product_nid");
	}
}