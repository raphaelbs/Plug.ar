angular.module('app', ['ui.router', 'ngMaterial', 'ngMessages']);

angular.module('app').config(['$stateProvider', '$urlRouterProvider', '$locationProvider', '$mdThemingProvider', function($stateProvider, $urlRouterProvider, $locationProvider, $mdThemingProvider) {

	$mdThemingProvider.theme('default')
	.primaryPalette('indigo')
	.accentPalette('red');

	$locationProvider.html5Mode({
		enabled: true,
		requireBase: false
	});

	$urlRouterProvider.otherwise('/');

	$stateProvider
	.state('home', {
		url: '/',
		views : {
			'main' : {
				templateUrl : '/views/home.html'
			}
		}
	});
}]);

angular.module('app').controller('app', ['$scope', '$timeout', function($scope, $timeout){
	var gProvider = new firebase.auth.GoogleAuthProvider();
	$scope.users = [];

	var ledStatus = firebase.database().ref('LED');
	ledStatus.on('value', function(v) {
		$scope.light.update(v.val());
	});
	var usersStatus = firebase.database().ref('users');
	usersStatus.on('child_added', function(v) {
		$scope.users.push({
			displayName: v.val().displayName,
			photoURL: v.val().photoURL,
			action: v.val().status,
			date: v.val().date
		});
		$scope.light.update(v.val().status);
	});

	function error(err){
		$timeout(function(){
			console.error(err);
			$scope.loging = false;
		});
	}

	function logoff(){
		delete $scope.user;
		$scope.loging = false;
	}

	function login(result){
		$scope.user = result.user;
		$scope.loging = false;

		function changeLight(status){
			firebase.database().ref('users').push({
			    displayName: $scope.user.displayName,
			    photoURL: $scope.user.photoURL,
				action: status,
				date: firebase.database.ServerValue.TIMESTAMP
		  	});
		}
		$scope.change = changeLight;
	}

	$scope.session = function(){
		$scope.loging = true;
		if($scope.user){
			firebase.auth().signOut().then(function() {
				$timeout(logoff, 0);
			}, error);
			return;
		}
		firebase.auth().signInWithPopup(gProvider).then(function(result) {
			var token = result.credential.accessToken;
			$timeout(function(){
				login(result);
			}, 0);
		}).catch(error);
	};

	$scope.light = { class: "loading", caption: 'carregando...', status: false,
 		change: function(){
			if(this.class === "loading") return;
			this.update();
			$scope.change && $scope.change(this.status);
		},
		update: function(s){
			if(s == undefined) s = this.status;
			this.status = !this.status;
			if(this.status) {
				this.class = "on";
				this.caption = "Acesa!";
			}else {
				this.class = "off";
				this.caption = "Apagada!";
			}
		}};

}]);

//directive para atualizar a data
angular.module('app').directive('relativeDate', ['$timeout', function($timeout) {

	function updateText(long){
		var d = new Date(long);
		if(typeof d == 'string') return '*data-invalida*';
		var diff = new Date() - d;
		if(diff<60000) return 'segundos atrás';
		if(diff<120000) return '1 minuto atrás';
		if(diff<3600000) return Math.round(diff/60000) + ' minutos atrás';
		if(diff<7200000) return '1 hora atrás';
		if(diff<86400000) return Math.round(diff/3600000) + ' horas atrás';
		var o = (d.getDate() < 10 ? '0' : '') + d.getDate() + '/';
		o += (d.getMonth() < 9 ? '0' : '') + (d.getMonth() + 1) + '/';
		o += d.getFullYear();
		return o;
	}

	function update(scope, element) {
		element.text(updateText(scope.actualTime));
		$timeout(function() { update(scope, element); }, 10000);
	}

	return {
		scope: {
			actualTime: '=relativeDate'
		},
		link: update
	};
}]);
