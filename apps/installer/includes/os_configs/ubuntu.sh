if ! command -v lsb_release &>/dev/null ; then
    sudo apt-get install -y lsb-release
fi

UBUNTU_VERSION=$(lsb_release -sr);

sudo apt update

# shared deps

sudo apt-get -y install make cmake clang curl unzip libmysqlclient-dev libace-dev git gcc g++ libssl-dev libbz2-dev libreadline-dev libncurses-dev mysql-server libace-dev

# Insstall boost 1.74 from ppa:mhier/libboost-latest for all os versions
sudo apt-get -y install libboost1.74-dev

if [[ $CONTINUOUS_INTEGRATION]]; then
  sudo add-apt-repository -y ppa:mhier/libboost-latest && sudo apt update && sudo apt-get -y install build-essential cmake-data  \
  libboost1.74-dev libbz2-dev libncurses5-dev libmysql++-dev libgoogle-perftools-dev libreadline6-dev libssl-dev libtool mysql-client \
  openssl zlib1g-dev
else
  case $UBUNTU_VERSION in
     "20.04")
       sudo apt-get install -y g++ gdb gdbserver gcc git \
       libace-6.* libboost-all-dev libbz2-dev libncurses-dev libreadline-dev \
       libssl-dev mysql-server 
       ;;
     *)
       sudo add-apt-repository -y ppa:mhier/libboost-latest && sudo apt update && sudo apt-get install -y g++ gdb gdbserver gcc git \
       libace-6.* libboost-all-dev libbz2-dev libncurses-dev libreadline-dev \
       libssl-dev mysql-server 
       ;;
  esac
fi