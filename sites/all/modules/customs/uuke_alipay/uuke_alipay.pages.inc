<?php

function uuke_alipay_configue_form($form, &$form_state) {
  $form['uuke_alipay_partner'] = array(
    '#type' => 'textfield',
    '#title' => t('支付宝的PID'),
    '#description' => t('合作者身份-你的支付宝商家服务中的合作者身份.'),
    // '#default_value' => variable_get('uuke_alipay_partner', ''),
    '#size' => 16,
  );
  $form['uuke_alipay_key'] = array(
    '#type' => 'textfield',
    '#title' => t('交易安全校验码'),
    '#description' => t('你的支付宝商家服务中的交易安全校验码.'),
    // '#default_value' => variable_get('uuke_alipay_key', ''),
    '#size' => 16,
  );
  $form['uuke_alipay_seller_email'] = array(
    '#type' => 'textfield',
    '#title' => t('支付宝帐户.'),
    '#description' => t('卖家支付宝帐户.'),
    // '#default_value' => variable_get('uuke_alipay_seller_email', ''),
  );
  $form['uuke_alipay_service'] = array(
    '#title' => t('支付方法'),
    '#type' => 'radios',
    '#default_value' => variable_get('uuke_alipay_service', 'create_partner_trade_by_buyer'),
    '#description' => t('您需要在支付宝商家服务中申请相关的服务.'),
    '#options' => array(
      'create_direct_pay_by_user' => t('支付宝即时到账交易接口'),
      'create_partner_trade_by_buyer' => t('支付宝纯担保交易接口'),
    )
  );  
  return system_settings_form($form);
}

/**
 * 
 * 
 */
function uuke_alipay_content_types_setting_form($value=''){
  $form = array();
  $form['titles'] = array(
    '#markup' => t('可以购买的节点的类型,自动添加Price字段field_uuke_price to the content type')
  );
  $synch_types = variable_get('uuke_alipay_content_types', NULL);
  if(!$synch_types){
    $synch_types = array();
  }else{
    $synch_types = array_filter(unserialize($synch_types));
  }
  $node_types = node_type_get_types();
  foreach ($node_types as $node_type => $node_type_obj) {
    if($node_type == 'uuke_alipay_order') continue;
    $form['node_types']["$node_type"] = array(
      '#title'=>$node_type_obj->name,
      '#type' => 'checkbox',
      '#default_value' =>key_exists($node_type_obj->type, $synch_types)?TRUE:FALSE,
      '#description' => $node_type_obj->description,
    );
  }
  // Add the buttons.
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 5,
  );
  $form['actions']['reset'] = array(
    '#type' => 'button',
    '#value' => t('reset'),
    '#weight' => 6,
  );
  return $form;
}

/**
 * Form submit handler for the open_users_synch_setting() form.
 */
function uuke_alipay_content_types_setting_form_submit($form, &$form_state){
  $node_types = node_type_get_types();
  $setting = array();
  foreach ($node_types as $node_type => $node_type_obj){
    if($node_type == 'uuke_alipay_order') continue;
    $setting[$node_type] = $form_state['values'][$node_type];
  }
  $synch_types = serialize($setting);
  if(variable_get('uuke_alipay_content_types', null)==$synch_types){
    drupal_set_message(t('设置未更改！'),'status');
  }else{
    variable_set('uuke_alipay_content_types', $synch_types);
    foreach ($setting as $content_type => $value) {
      $data = array(
        'instances' => array(
          'field_uuke_price' => array(
            0 => array(
              'label' => 'Price',
              'widget' => array(
                'type' => 'number',
                'weight' => '5',
                'settings' => array(),
                'module' => 'number',
              ),
              'settings' => array(
                'min' => '',
                'max' => '',
                'prefix' => '',
                'suffix' => '',
                'user_register_form' => FALSE,
              ),
              'display' => array(
                'default' => array(
                  'label' => 'above',
                  'type' => 'number_decimal',
                  'settings' => array(
                    'thousand_separator' => ' ',
                    'decimal_separator' => '.',
                    'scale' => 2,
                    'prefix_suffix' => TRUE,
                  ),
                  'module' => 'number',
                  'weight' => 9,
                ),
                'teaser' => array(
                  'type' => 'hidden',
                  'label' => 'above',
                  'settings' => array(),
                  'weight' => 0,
                ),
              ),
              'required' => FALSE,
              'description' => '',
              'field_name' => 'field_uuke_price',
              'entity_type' => 'node',
              'bundle' => $content_type,
              'deleted' => '0',
              'default_value' => NULL,
            ),
          ),
        ),
        'fields' => array(
          'field_uuke_price' => array(
            'translatable' => '0',
            'entity_types' => array(),
            'settings' => array(
              'precision' => '10',
              'scale' => '2',
              'decimal_separator' => '.',
            ),
            'storage' => array(
              'type' => 'field_sql_storage',
              'settings' => array(),
              'module' => 'field_sql_storage',
              'active' => '1',
              'details' => array(
                'sql' => array(
                  'FIELD_LOAD_CURRENT' => array(
                    'field_data_field_uuke_price' => array(
                      'value' => 'field_uuke_price_value',
                    ),
                  ),
                  'FIELD_LOAD_REVISION' => array(
                    'field_revision_field_uuke_price' => array(
                      'value' => 'field_uuke_price_value',
                    ),
                  ),
                ),
              ),
            ),
            'foreign keys' => array(),
            'indexes' => array(),
            'field_name' => 'field_uuke_price',
            'type' => 'number_decimal',
            'module' => 'number',
            'active' => '1',
            'locked' => '0',
            'cardinality' => '1',
            'deleted' => '0',
            'columns' => array(
              'value' => array(
                'type' => 'numeric',
                'precision' => '10',
                'scale' => '2',
                'not null' => FALSE,
              ),
            ),
            'bundles' => array(
              'node' => array(
                0 => 'app',
                1 => 'page',
              ),
            ),
          ),
        ),
      );

      if($value) {
        foreach ($data['fields'] as $field_name => $field) {
          $ori_field = field_info_field($field_name);
          if (empty($ori_field)) {
             $field = field_create_field($field);
             drupal_set_message(t('created field for uuke_alipay_order: ').$field_name, 'status', FALSE);
          }
        }
        //add instances
        foreach ($data['instances'] as $instances_name => $instances) {
          foreach ($instances as $key => $instance) {
            //$instance = field_info_instance('node', 'body', $type->type);
            $ori_instance = field_info_instance('node', $instances_name, $instance['bundle']);
            if (empty($ori_instance)) {
               $instance = field_create_instance($instance);
               drupal_set_message(t('created instance '.$instances_name.' for ').$content_type, 'status', FALSE);
            }
          }
        }
      }else{//del instances

        
      }
    }
    drupal_set_message(t('设置成功！'),'status');
    // drupal_set_message(t('请手动删除取消的content_typ field_uuke_price字段'),'status');
  }
}

  /**
   * @file
   *集成支付宝的支付服务.
   *zhupou.cn制作
   */

function uuke_alipay_notify() {
  $request = $_REQUEST;//_POST
  //$output = "uc_alipay_return test";
  $partner = variable_get('uuke_alipay_partner', '');
  $security_code = variable_get('uuke_alipay_key', '');
  $sign_type = "MD5";
  $_input_charset = 'UTF-8';
  $transport = 'https';
  if(!isset($request['out_trade_no'])) {
    watchdog('uc_alipay_notify_called at2', REQUEST_TIME);      
    echo 'fail';
    return;
  }
  module_load_include('inc', 'uuke_alipay', 'uuke_alipay_service');
  $alipay = new alipay_notify($partner,$security_code,$sign_type,$_input_charset,$transport);
  $verify_result = $alipay->notify_verify();
  $out_trade_no   = $request['out_trade_no'];   //获取支付宝传递过来的订单号
  $trade_status = $request['trade_status'];//WAIT_BUYER_PAY
  $trade_no =  $request['trade_no'];
  if($verify_result) {  //认证合格
    //获取支付宝的反馈参数
    $order_nid = $out_trade_no - TRADE_ADD; //20130420804-20130420802
    $order_node = node_load((int) $order_nid);
  
    // $order_node = node_load($out_trade_no-20130420802);
    // $product_nid = $order_node->field_uuke_alipay_product_nid[LANGUAGE_NONE][0]['target_id'];
    // $product_node = node_load((int)$product_nid);
    $nid = $order_node->nid;
    $buy_timestamp = $order_node->created;
    
    $total_fee     = $request['total_fee'];      //获取支付宝传递过来的总价格
    $price =  $request['price'];
    $quantity =  $request['quantity'];
    $trade_status_array = array(
      // 'WAIT_BUYER_PAY',
      'TRADE_FINISHED',
      'TRADE_SUCCESS',
      'WAIT_SELLER_SEND_GOODS',
      'WAIT_BUYER_CONFIRM_GOODS',
      // 'TRADE_CLOSED',
    );
    if(in_array($trade_status,$trade_status_array)) {
      //执行自动发货 begin
      uuke_alipay_auto_send_goods($trade_no,$out_trade_no);
      watchdog('uuke_alipay_auto_send_goods', '$order->nid =>'.$order_node->nid, array(), WATCHDOG_NOTICE, 'link');
      //执行自动发货 end
    }
    // else{
    //   echo "success";//不再接收此通知 //实际上是error
    //   return;
    // }
    // $receive_name    =$request['receive_name'];    //获取收货人姓名
    // $receive_address =$request['receive_address']; //获取收货人地址
    // $receive_zip     =$request['receive_zip'];     //获取收货人邮编
    // $receive_phone   =$request['receive_phone'];   //获取收货人电话
    // $receive_mobile  =$request['receive_mobile'];  //获取收货人手机
    /* 
       获取支付宝反馈过来的状态,根据不同的状态来更新数据库 
       WAIT_BUYER_PAY(表示等待买家付款);
       TRADE_FINISHED(表示交易已经成功结束);
      */

    //更新订单
    if($order_node->field_uuke_alipay_trade_status[LANGUAGE_NONE][0]['value'] != uuke_alipay_trade_status_to_chinese($trade_status)) {
      $order = uuke_alipay_order_save($request,$order_node->nid);
      watchdog('订单更新','$order->nid =>'.$order_node->nid);
    }
   
    echo 'success';return;
  }else {    //认证不合格
    watchdog('uc_alipay_notify', $trade_status.':认证不合格,out_trade_no='.$out_trade_no.'&trade_no='.$trade_no);
    echo "fail";return;
    //log_result ("verify_failed");
  }
}

function uuke_alipay_return() {
  //获取支付宝的反馈参数
  if(!isset($_GET['out_trade_no'])) return 'wrong!';
  $out_trade_no   = $_GET['out_trade_no'];   //获取支付宝传递过来的订单号
  $order_nid = $out_trade_no - TRADE_ADD;
  $order_node = node_load((int) $order_nid);

  $product_node = node_load($order_node->field_uuke_alipay_product_nid[LANGUAGE_NONE][0]['target_id']);
  $trade_no =  $_GET['trade_no'];
  $trade_status = $_GET['trade_status'];
  // $body = $_GET['body'];
  $partner = variable_get('uuke_alipay_partner', '');
  $security_code = variable_get('uuke_alipay_key', '');
  $sign_type = "MD5";
  $_input_charset = 'UTF-8';
  $transport = 'https';
  module_load_include('inc', 'uuke_alipay', 'uuke_alipay_service');
  $alipay = new alipay_notify($partner,$security_code,$sign_type,$_input_charset,$transport);
  $verify_result = $alipay->return_verify();

  // dpm($_GET,'$_GET');
  if($verify_result) {    //认证合格
    if($trade_status == 'WAIT_SELLER_SEND_GOODS') {
      // create a now order node begin
      $request = $_GET;

      $order = uuke_alipay_order_save($request,$order_node->nid);
      //执行自动发货 begin
      uuke_alipay_auto_send_goods($trade_no,$out_trade_no);
      //执行自动发货 end
      // create a now order node end
      drupal_set_message(t('订单'.$product_node ->title.':'.$order_node->title.' 支付成功,请积极联系卖家以便完成交易。'), 'status', FALSE);
      drupal_goto('node/'.$product_node->nid);
    }elseif($trade_status == 'TRADE_FINISHED' || $trade_status == 'TRADE_SUCCESS' || $trade_status=='WAIT_BUYER_CONFIRM_GOODS') {
    //这里放入你自定义代码,比如根据不同的trade_status进行不同操作
    //log_result("verify_success"); 
      // drupal_set_message(t('已购买，请点此下载安装App.'), 'status', FALSE);
      drupal_goto('node/'.$product_node->nid);
    }else{
      drupal_set_message(t('unknow error: Please conneect admin. trade_status:'.$_GET['trade_status']), 'warning', FALSE);
      drupal_goto('node/'.$product_node->nid);
    }
  }
  else {//认证不合格
    $output = t('您的交易信息是无效的，如果你使用的是已有的返回信息的话，那么交易已经成立，你可以查看订单历史。如果你是黑客的话，俺是守法的好公民，希望你能饶了我这个小店');
    //echo "fail";
    //log_result ("verify_failed");
  }
  return '';
}

