var gulp        = require('gulp'),
    shell       = require('gulp-shell');


gulp.task('run', ['makerom'],
    shell.task([
            'mess.exe a7800 -cart "..\\..\\build\\game.bas.a78"'
        ],
        {
            cwd : 'resources/mess'
        })
    );

gulp.task('makerom',
    shell.task([
        '7800bas.bat game.bas',
        'copy *.a78 ..\\build',
        'copy *.bin ..\\build'
        ], {
            cwd : 'src'
        })
);

gulp.task('default', ['makerom']);