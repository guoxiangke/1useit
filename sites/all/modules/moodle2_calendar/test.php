<?
function my_html4js($s){
	return str_replace(array("'",'"','&',"\\\\","\\n","\\r","\\t","\\b","\\f","\n","\r","\t","\b","\f"), array("\\'",'\\"',"\\&",'\\\\',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '),$s);
}

echo my_html4js('<ul>
<li>intro of term end test quiz</li>
</ul>
<ul>
<li>term end test quiz</li>
</ul> (Open Date)');
?>
