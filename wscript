top = '.'
out = 'build'

def options(opt):
    opt.load('compiler_c')
    opt.load('compiler_cxx')

def configure(conf):
    print('	configuring the project in ' + conf.path.abspath())
    conf.load('compiler_c')
    conf.load('compiler_cxx')
    conf.check_cfg(package='gtkmm-3.0', args='--cflags --libs', uselib_store='GTKMM', mandatory=1)
    conf.check_cfg(package='glibmm-2.4', args='--cflags --libs', uselib_store='GLIBMM', mandatory=1)
    conf.check_cfg(package='jack', args='--cflags --libs', uselib_store='JACK', mandatory=1)

def build(bld):
    bld(rule='cp ${SRC} ${TGT}', source='src/reFactor.glade', target='reFactor.glade')

    bld.objects(source='src/jack_stuff.c',
		target='jackstuff',
		uselib='JACK')

    bld.objects(source='src/midi.cc',target='midi',use=['jackstuff'])

    bld.objects(source='src/sysex_parser.cc',target='sysex_parser')

    bld.objects(source='src/refactor.cc',
		target='refactor',
		use=['midi', 'sysex_parser'],
		uselib='GTKMM GLIBMM')

    bld.program(source='src/main.cc',
		target='factorx',
		use=['refactor'],
		uselib=('GTKMM GLIBMM')
		)
