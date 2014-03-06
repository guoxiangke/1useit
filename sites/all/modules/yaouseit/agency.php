<?php
class agency
{
	function __construct()
	{
		
	}
	//回放测试实例
	public function test_replay()
	{
		header ( 'content-type:text/html;charset=utf-8' );
		$client_info = array (	
				'room' => 'test-1', //必填
				'room_title' => '课程名称',//必填
				'key' => 'test51a5a9a22a8b7', //必填
				'company_code' => 'test'
		);//必填
		$this->replay($client_info) ;
	}
	//直播测试实例
	public function test_live()
	{
		header ( 'content-type:text/html;charset=utf-8' );
		$client_info = array (
				'name' => '参加人的昵称或姓名', //必填
				'userid' => 1, //必填
				'role' => 1, 		// 角色，1是主讲，2是听众
				'room' => 'test-1', //必填
				'room_title' => '课程名称',//必填
				'avatar' => 'http://www.tetequ.com/statics/images/nophoto.jpg',//选填
				'key' => 'test51a5a9a22a8b7', //必填
				'company_code' => 'test' //必填
		);//必填
		$this->live ( $client_info );
	}
	/**
	 * 客户端直播
	 */
	public function live($client_info)
	{
		//直播接口地址
		$live_url = 'http://www.tetequ.com/api/agency_live';
		//获取直播接入码
		$json_live_code = $this->get_live_code ( $client_info ['key'], $client_info ['company_code'] );
		//得到是json数据
		$live_code = json_decode ( $json_live_code );
		// $live_code[status]如果是1，2，3，4都是出错，打印错误；如果是5，则正确，返回直播码；
		if ($live_code->status == 5)
		{
			//注销key,防止POST传输
			unset ( $client_info ['key'] );
			//设置直播码
			$client_info ['live_code'] = $live_code->live_code;
			//利用form表单进行数据传输
			$form = "<form  method ='post' name = 'live_connect' action='{$live_url}'>";
			
			foreach ( $client_info as $k => $v )
			{
				$form .= "<input name ='$k' value='$v' type='hidden'/>";
			}
			$form .= '</form>';
			
			$form .= '<script>';
			$form .= '  document.live_connect.submit();';
			$form .= '</script>';
			echo $form;
		}
		else
		{
			//错误则显示错误信息
			echo $live_code ->message;
		}
	}
	/**
	 * 客户端回放
	 */
	public function replay($client_info)
	{
		//录播回放接口地址
		$replay_url = 'http://www.tetequ.com/api/agency_replay';
		//获取直播接入码
		$json_live_code = $this->get_live_code ( $client_info ['key'], $client_info ['company_code'] );
		//得到的是json格式的数据
		$live_code = json_decode ( $json_live_code );
		// $live_code[status]如果是1，2，3，4都是出错，打印错误；如果是5，则正确，返回直播码；
		if ($live_code->status == 5)
		{
			//注销key,防止POST传输
			unset ( $client_info ['key'] );
			//设置直播码，传送到直播接口	
			$client_info ['live_code'] = $live_code->live_code;
			//利用form表单实现POST数据传送
			$form = "<form  method ='post' name = 'replay_connect' action='{$replay_url}'>";
	
			foreach ( $client_info as $k => $v )
			{
				$form .= "<input name ='$k' value='$v' type='hidden'/>";
			}
			$form .= '</form>';
	
			$form .= '<script>';
			$form .= '  document.replay_connect.submit();';
			$form .= '</script>';
			echo $form;
		}
		else
		{
			//输出错误信息
			echo $live_code->message;
		}
	}
	/**
	 * 通过key和company_code取得直播码
	 * 通过这种方式传送key和company_code,保护客户的key和company_code
	 * @param
	 *        	$key
	 * @param
	 *        	$company_code
	 */
	public function get_live_code($key, $company_code)
	{
		$info ['key'] = $key;
		$info ['company_code'] = $company_code;
		// 如果支持curl,则用curl获取直播码
		if (function_exists ( 'curl_init' ))
		{
			$ch = curl_init ( 'http://www.tetequ.com/api/get_live_code' );
			curl_setopt ( $ch, CURLOPT_RETURNTRANSFER, TRUE );
			curl_setopt ( $ch, CURLOPT_POST, TRUE );
			curl_setopt ( $ch, CURLOPT_POSTFIELDS, http_build_query ( $info ) );
			curl_setopt ( $ch, CURLOPT_HEADER, FALSE );
			$result = curl_exec ( $ch );
			return $result;
		}
		else
		{
			//利用file_get_contents获取直播码
			$result = file_get_contents ( 'http://www.tetequ.com/api/get_live_code/?' . http_build_query ( $info ) );
			return $result;
		}
	}
}

?>