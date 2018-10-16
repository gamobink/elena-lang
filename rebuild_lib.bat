bin\sg dat\sg\syntax.txt
move dat\sg\syntax.dat bin

bin\og dat\og\rules.txt
move dat\og\rules.dat bin

bin\asm2binx src34\core\core_routines.esm lib34\system
bin\asm2binx asm\x32\core.asm bin\x32
bin\asm2binx asm\x32\corex.asm bin\x32                
bin\asm2binx asm\x32\coreapi.asm bin\x32
bin\asm2binx asm\x32\core_win.asm bin\x32

rem bin\asm2binx -amd64 asm\amd64\core.asm bin\amd64
rem bin\asm2binx -amd64 asm\amd64\core_win.asm bin\amd64
rem bin\asm2binx -amd64 asm\amd64\coreapi.asm bin\amd64

bin\elc src34\system\system.prj
bin\elc src34\extensions\extensions.prj
bin\elc src34\net\net.prj
rem bin\elc src33\forms\forms.prj
bin\elc src34\sqlite\sqlite.prj
bin\elc src34\cellular\cellular.prj
rem bin\elc src33\graphics\graphics.prj
rem bin\elc src33\xforms\xforms.prj

rem bin\elc src31\system\system_64.prj
