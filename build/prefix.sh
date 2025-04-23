export PATH=/ucrt64/bin:$PATH

# LD_LIBRARY_PATH only needed if you are building without rpath
# export LD_LIBRARY_PATH=/ucrt64/lib:$LD_LIBRARY_PATH

export XDG_DATA_DIRS=/ucrt64/bin/data:${XDG_DATA_DIRS:-/usr/local/share:/usr/share}
export XDG_CONFIG_DIRS=/ucrt64/etc/xdg:${XDG_CONFIG_DIRS:-/etc/xdg}

export QT_PLUGIN_PATH=/ucrt64/lib/plugins:$QT_PLUGIN_PATH
export QML2_IMPORT_PATH=/ucrt64/lib/qml:$QML2_IMPORT_PATH

export QT_QUICK_CONTROLS_STYLE_PATH=/ucrt64/lib/qml/QtQuick/Controls.2/:$QT_QUICK_CONTROLS_STYLE_PATH

export MANPATH=/ucrt64/bin/data/man:${MANPATH:-/usr/local/share/man:/usr/share/man}

export SASL_PATH=/ucrt64/lib/sasl2:${SASL_PATH:-/usr/lib/sasl2}
