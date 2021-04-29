angular.module('sagremorApp')

.directive('gridstack', ['$timeout', function($timeout) {

    return {
      restrict: "A",
      controller: 'GridstackController',
      scope: {
        onChange: '&',
        onDragStart: '&',
        onDragStop: '&',
        onResizeStart: '&',
        onResizeStop: '&',
        options: '='
      },
      link: function (scope, element, attrs, controller, ngModel) {

        controller.init(element, scope.options);

        element.on('change', function (e, items) {
          $timeout(function() {
            scope.$apply();
            scope.onChange({event: e, items: items});
          });
        });

        element.on('dragstart', function(e, ui) {
          scope.onDragStart({event: e, ui: ui});
        });

        element.on('dragstop', function(e, ui) {
          $timeout(function() {
            scope.$apply();
            scope.onDragStop({event: e, ui: ui});
          });
        });

      }
    };

}]).directive('gridstackItem', ['$timeout', function($timeout) {

    return {
      restrict: "A",
      controller: 'GridstackController',
      require: '^gridstack',
      scope: {
        gridstackItem: '=',
        onItemAdded: '&',
        onItemRemoved: '&',
        gsItemX: '=',
        gsItemY: '=',
        gsItemWidth: '=',
        gsItemHeight: '=',
        gsItemAutopos: '='
      },
      link: function (scope, element, attrs, controller) {

        $(element).attr('data-gs-x', scope.gsItemX);
        $(element).attr('data-gs-y', scope.gsItemY);
        $(element).attr('data-gs-width', scope.gsItemWidth);
        $(element).attr('data-gs-height', scope.gsItemHeight);
        $(element).attr('data-gs-auto-position', scope.gsItemAutopos);
        var widget = controller.addItem(element);
        var item = element.data('_gridstack_node');
        $timeout(function() {
          scope.onItemAdded({item: item});
        });

        scope.$watch(function(){ return $(element).attr('data-gs-x'); }, function(val) {
          scope.gsItemX = val;
        });

        scope.$watch(function(){ return $(element).attr('data-gs-y'); }, function(val) {
          scope.gsItemY = val;
        });

        scope.$watch(function(){ return $(element).attr('data-gs-width'); }, function(val) {
          scope.gsItemWidth = val;
        });

        scope.$watch(function(){ return $(element).attr('data-gs-height'); }, function(val) {
          scope.gsItemHeight = val;
        });

        element.bind('$destroy', function() {
          var item = element.data('_gridstack_node');
          scope.onItemRemoved({item: item});
          controller.removeItem(element);
        });

      }

    };

  }]);
