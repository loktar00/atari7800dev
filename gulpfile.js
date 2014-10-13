var gulp        = require('gulp'),
    shell       = require('gulp-shell'),
    imagemin    = require('gulp-imagemin');

gulp.task('imageopt', function(){
    gulp.src('src/gfx/unopt/*.png')
        .pipe(imagemin({
            progressive: false
        }))
        .pipe(gulp.dest('src/gfx'));
    });

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

gulp.task('default', ['run']);