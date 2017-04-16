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

	var firebaseEvents = {
		assigned : false,
		fns: function(){
			if(this.assigned) return;
			firebase.database().ref('users').on('child_added', function(v) {
				$scope.users.push({
					displayName: v.val().displayName,
					photoURL: v.val().photoURL,
					action: v.val().action,
					date: v.val().date
				});
			});
			firebase.database().ref('LED').on('value', function(v) {
				$scope.light.update(v.val());
			});
			this.assigned = true;
		}
	};

	// Verifica se está logado
	if(localStorage.getItem('user')){
		login(JSON.parse(localStorage.getItem('user')));
	}

	function error(err){
		$timeout(function(){
			console.error(err);
			$scope.loging = false;
		});
	}

	function logoff(){
		delete $scope.user;
		$scope.loging = false;
		localStorage.removeItem('user');
	}

	function login(result){
		$scope.user = {
			displayName: result.displayName,
			photoURL: result.photoURL
		};
		localStorage.setItem('user', JSON.stringify($scope.user));
		$scope.loging = false;

		firebaseEvents.fns();

		function changeLight(status){
			firebase.database().ref('users').push({
			    displayName: $scope.user.displayName,
			    photoURL: $scope.user.photoURL,
				action: status,
				date: firebase.database.ServerValue.TIMESTAMP
		  	});
			firebase.database().ref('LED').transaction(function(){ return status; })
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
				login(result.user);
			}, 0);
		}).catch(error);
	};

	$scope.light = { class: "loading", caption: 'carregando...', status: false,
 		change: function(){
			if(this.class === "loading") return;
			$scope.change && $scope.change(!this.status);
		},
		update: function(s){
			var self = this;
			self.status = s;
			$timeout(function(){
				if(self.status) {
					self.class = "on";
					self.caption = "Acesa!";
				}else {
					self.class = "off";
					self.caption = "Apagada!";
				}
			},0);
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
