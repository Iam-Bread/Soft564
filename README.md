READ ME FILE 

To Run the Code you first must enter your wifi network credentials near the top of the esp code
Next select the board on the arduino IDE NodeMCU-32s and connected comport 
now upload the code to the board

For the arduino uno, select the board on the IDE and comport and upload.

When the code starts it will try connect to the wifi 
If this fails then it will start bluetooth instead
If you wish to retry connecting to wifi click the EN button on the esp-32

When connected the LCD screen will display the IP address , which can be entered into a browser to open the webpage

If you want bluetooth instead, click the enable button if its connected to wifi and it will switch
On a Bluetooth serial terminal app connect to the device "Soft564 Robot"
typing "help" in the terminal and sending it will print a list of available commands 

Using the IR Remote it is possible to control the robot regardless if the wifi or bluetooth is connected by using the arrow keys and ok button.
