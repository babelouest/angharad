var config = require('./config');

exports.config = require('gulp-app-build-tasks')
    .protractor(config);