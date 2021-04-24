
if ! command -v lsb_release &>/dev/null ; then
       sudo apt-get install -y lsb-release
fi

UBUNTU_VERSION=$(lsb_release -sr);

sudo apt-get update -y

# shared deps
sudo apt-get -y install make cmake clang curl unzip libmysqlclient-dev libace-dev
sudo apt-get install -y git gcc g++ libssl-dev libbz2-dev libreadline-dev libncurses-dev mysql-server libace-6.*
