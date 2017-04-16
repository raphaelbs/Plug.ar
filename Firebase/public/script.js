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

	function error(err){
		$timeout(function(){
			console.error(err);
			$scope.loging = false;
		});
	}

	$scope.session = function(){
		$scope.loging = true;
		if($scope.user){
			firebase.auth().signOut().then(function() {
				$timeout(function(){
					delete $scope.user;
					$scope.loging = false;
				});
			}, error);
			return;
		}
		firebase.auth().signInWithPopup(gProvider).then(function(result) {
			var token = result.credential.accessToken;
			$timeout(function(){
				$scope.user = result.user;
				$scope.loging = false;
			});
			console.log(result);
		}).catch(error);
	};

	$scope.color = { color: "'rgba(0,0,0,0;26)'", caption: 'carregando...'};

}]);


/*global angular:true*/
/**
 * [brDate]
 * Filtro para regularizar as datas no modo brasileiro.
 * Ex: {{123123 | brDate}} = 01/02/2017
 */
angular.module('app').filter('brDate', function() {
	function doDate(long){
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
	return function(input) {
		if(input == undefined) return '*sem-data*';
		if(input.match(/^\d+$/g)) return doDate(input);
		if(input.match(/^\w{24,24}$/g)) return doDate(parseInt(input.substring(0, 8), 16)*1000);
		return doDate(input);
	};
});
