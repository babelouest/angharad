var gulp = require("gulp");
var config = require("./config");
var gulpAppBuildTasks = require("gulp-app-build-tasks");
var build = require("gulp-build");

gulpAppBuildTasks.apply(config, gulp);
