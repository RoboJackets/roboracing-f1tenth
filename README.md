# Welcome to RoboRacing's 2025 F1tenth Season

## Team Members: 
- Eshan Jaffar: Project Manager
- Andrew Son: Software Lead
- Zane Pressley: Mechatronic Lead

Software Team: 
- Sasanka Polisetti
- Justin Gardiner
- Cipriano Dorbessan
- Daniel Thompson
- Rohan Nair
- Nathan Donagi
- Eshan Jaffar
- Andrew Son

Mechanical Team: 
- Ian Causseaux

Electronic: 
- Jackie Mac Hale
- Zane Pressley

## 2025 Notes:

### Current Simulation Instructions
#### Installation
Link to F1tenth simulation: https://github.com/f1tenth/f1tenth_gym_ros
1. First go down to the section "Without an NVIDIA gpu" if you are on MacOS or Windows without an   NVIDIA gpu.
2. Follow instructions as in the read me (make sure to clone f1tenth_gym_ros)
3. When running the setup scripts make sure to run them in a separate terminal in the "Launching the Simulation section"
#### Configure Map
Download real F1 tracks to test with here: https://github.com/f1tenth/f1tenth_racetracks
To change the map:
1. In the f1tenth_gym_ros repository, upload the track's png and yaml files.
2. Edit the sim.yaml located in the config folder of the f1tenth_gym_ros repository
  - Change the map_path parameter to reflect the name of track you want to use
  - Edit the starting position and angle of the ego driver to match the track (eyeball it for now)

### History:
F1-tenth + AKS autonomous racing starting 2024
previously: [software repo](https://github.com/RoboJackets/roboracing-software)

### UDP connection setup Jetson Orin Nano
1. Install netcat on Jetson if it doesn't already exist (check if package exists with command nc)
2. Run this command to set manual mode ```sudo nmcli c mod [CONNECTION_NAME] ipv4.method manual``` where [CONNECTION_NAME] is replaced with the name of your WiFi connection that you got from step 8.
3. Set static ip, preferably as 192.168.20.[some number here] using this command: ```sudo nmcli c mod [CONNECTION_NAME] ipv4.address [NEW_ADDRESS]/[CIDR]```
4. Run this command to send UDP packets ```echo "Hello World" | nc -u 192.168.20.2 8888```



### Ethernet Connection (Windows 11)

- Windows Firewall Inbound Rule
Before testing UDP connections via ethernet cable. Make sure that 'Windows Defender Firewall with Advanced Security' is not blocking UDP connections to the Jetson Orin Nano.

- Creating an inbound rule
Refer to this tutorial: https://learn.microsoft.com/en-us/windows/security/operating-system-security/network-security/windows-firewall/configure

- Configuring IPV4 to allow for UDP Connections
For windows, go to Settings > Network & Internet > Ethernet > IP Assignment
1. Set 'Edit IP Settings' to Manual
2. Turn IPv4 on and set the 'IP address' to 192.168.20.4 (Make sure that two devices are not using the same IP address)

- Windows Firewall Inbound Rule
### Testing on Windows for sent UDP commands
To test UDP commands on windows, use the following commands below (make sure netcat is installed):
1. ssh roboracing@192.168.20.2
2. ncat -ul 8888 (to catch incoming UDP commands.) Listening
