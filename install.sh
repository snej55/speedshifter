# install kirigami packages
declare -A osInfo;
osInfo[/etc/redhat-release]="sudo dnf install @development-tools @development-libs cmake extra-cmake-modules kf6-kirigami2-devel kf6-ki18n-devel kf6-kcoreaddons-devel kf6-kiconthemes-devel qt6-qtbase-devel qt6-qtdeclarative-devel qt6-qtquickcontrols2-devel kf6-qqc2-desktop-style -y"
osInfo[/etc/arch-release]="sudo pacman -S base-devel extra-cmake-modules cmake kirigami ki18n kcoreaddons breeze kiconthemes qt6-base qt6-declarative qqc2-desktop-style -y --needed"
osInfo[/etc/SuSE-release]="sudo zypper install cmake kf6-extra-cmake-modules kf6-kirigami-devel kf6-ki18n-devel kf6-kcoreaddons-devel kf6-kiconthemes-devel qt6-base-devel qt6-declarative-devel qt6-quickcontrols2-devel kf6-qqc2-desktop-style -y"
osInfo[/etc/debian_version]="sudo apt install build-essential cmake extra-cmake-modules libkirigami-dev libkf6i18n-dev libkf6coreaddons-dev libkf6iconthemes-dev qt6-base-dev qt6-declarative-dev libkf6qqc2desktopstyle-dev -y"

for f in ${!osInfo[@]}
do
    if [[ -f $f ]];then
        echo Package manager: ${osInfo[$f]}
        eval ${osInfo[$f]}
    fi
done

# generate cmake files
cmake -B build/ --install-prefix ~/.local
# build
cmake --build build/ --parallel
sudo cmake --install build/
echo "Installed speedshifter to ~/.local/bin"