var config = require('./config');

module.exports = function (karmaConfig) {
    require('gulp-app-build-tasks')
        .karma(karmaConfig, config);
}