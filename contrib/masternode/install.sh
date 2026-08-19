#!/bin/bash
if free | awk '/^Swap:/ {exit !$2}'; then
echo "Have swap"
else
sudo touch /var/swap.img
sudo chmod 600 /var/swap.img
sudo dd if=/dev/zero of=/var/swap.img bs=1024k count=2000
mkswap /var/swap.img
sudo swapon /var/swap.img
sudo echo "/var/swap.img none swap sw 0 0" >> /etc/fstab
fi
sudo apt-get update -y
sudo apt-get upgrade -y
sudo apt-get dist-upgrade -y
sudo apt-get install mc htop git python-virtualenv ntpdate -y
sudo ntpdate -u pool.ntp.org
sudo mkdir /opt/subo-core
cd /opt/subo-core
wget https://github.com/subochain/subo/releases/download/0.13.0.1/subo-0.13.0.1-linux.tgz
tar -xvf subo-0.13.0.1-linux.tgz
rm subo-0.13.0.1-linux.tgz
mv subo-0.13.0.1-linux/subod ./subod
mv subo-0.13.0.1-linux/subo-cli ./subo-cli
mv subo-0.13.0.1-linux/subo-tx ./subo-tx
mv subo-0.13.0.1-linux/subo-qt ./subo-qt
rm -rf subo-0.13.0.1-linux
chmod -R 755 /opt/subo-core
cd /opt
git clone https://github.com/subochain/subo-sentinel subo-sentinel
cd subo-sentinel
virtualenv ./venv
./venv/bin/pip install -r requirements.txt
cat <(crontab -l) <(echo "* * * * * cd /opt/subo-sentinel && ./venv/bin/python bin/sentinel.py >/dev/null 2>&1") | crontab -
cd /opt/subo-core
./subod -daemon
sleep 10
masternodekey=$(./subo-cli masternode genkey)
./subo-cli stop
sleep 3
echo -e "\nserver=1\nlisten=1\ndaemon=1\nmaxconnections=256\nmasternode=1\nmasternodeprivkey=$masternodekey\nrpcuser=RPCUSER\nrpcpassword=RPCPASSWORD\nrpcport=9798\nrpcallowip=127.0.0.1\naddnode=194.154.27.42:9119\naddnode=130.17.13.125:9119\naddnode=89.127.207.196:9119\n" >> "/root/.subochain/subo.conf"
sleep 3
sudo sed -i -e "s/exit 0/sudo \-u root \/opt\/subo-core\/subod \> \/dev\/null \&\nexit 0/g" /etc/rc.local
./subod -daemon
echo "Masternode private key: $masternodekey"
echo "Job completed successfully"
