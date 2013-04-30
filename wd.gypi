{
  'variables': {
	'QT5': '0', # change to '1' for enabling Qt5
	'WD_CONFIG_QWIDGET_VIEW': '1',
    'WD_CONFIG_QWIDGET_VIEW_ACCESSABILITY': '0',
    'WD_CONFIG_QML_VIEW': '1',
    'WD_BUILD_MONGOOSE': '1',
    'WD_BUILD_MODP_B64': '0',
    'DESKTOP_QT_BIN_PATH': '/usr/lib/qt4/bin',
    'DESKTOP_QT_INC_PATH': '/usr/include',
    'DESKTOP_QT_LIB_PATH': '/usr/lib',
    'CISCO_QT_INC_PATH':   '/opt/qt4_nds/linux2_2/release/include',
    'CISCO_QT_LIB_PATH':   '/opt/qt4_nds/linux2_2/release/lib/',
    'CISCO_QT_BIN_PATH':   '/opt/qt4_nds/linux2_2/release/bin/',
    'CISCO_MONGOOSE_INC_PATH':    '<(ROOT_PATH)/MONGOOSE/dist/<(platform)/generic/<(mode)/h',
  },
}
