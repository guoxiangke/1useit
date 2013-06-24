(function($){
	var initLayout = function() {

		var now = new Date();
		now.setHours(0,0,0,0);
		//now.addDays(-10);
		//var now2 = new Date();
		//now2.addDays(+5);
		//now2.setHours(0,0,0,0);
		$('#date').DatePicker({
			flat: true,
			date: ['2013-05-09'],
			current: '2013-06-01',
			format: 'Y-m-d',
			calendars: 2,
			onRender: function(date) {
				return {
					disabled: (date.valueOf() < now.valueOf()),
					className: date.valueOf() < now.valueOf() ? 'datepickerSpecial' : false
				}
			},
			onChange: function(formated, dates) {
				alert(formated);
			},
			starts: 1
		});

	};
	
	$(initLayout);//initLayout();
	//EYE.register(initLayout, 'init');
})(jQuery)
