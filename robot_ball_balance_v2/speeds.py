import matplotlib.pyplot as plt
import numpy as np

sqrt32 = 3**(0.5)/2

angle = [i for i in range(-180, 181)]
xSpeed = []
ySpeed = []
robotSpeed = []
motor1Speed = []
motor2Speed = []
motor3Speed = []
for i in range(len(angle)):
    xSpeed.append(100*np.sin(angle[i]*np.pi/180))
    ySpeed.append(100*np.cos(angle[i]*np.pi/180))
    robotSpeed.append((xSpeed[i]**2+ySpeed[i]**2)**0.5)
    motor1Speed.append(ySpeed[i])
    motor2Speed.append(-xSpeed[i]*sqrt32 - ySpeed[i]/2)
    motor3Speed.append(+xSpeed[i]*sqrt32 - ySpeed[i]/2)

def plotSpeedsxSpeed(): #en fonction de x.speed
    plt.plot(xSpeed, robotSpeed, label = "|speed|")
    plt.plot(xSpeed, ySpeed, label = "y.speed")
    plt.plot(xSpeed, motor1Speed, label = "motor1.speed")
    plt.plot(xSpeed, motor2Speed, label = "motor2.speed")
    plt.plot(xSpeed, motor3Speed, label = "motor3.speed")
    plt.legend()
    plt.grid()
    plt.show()
    
def plotSpeedsDirection(): #en fonction de la direction
    plt.plot(angle, robotSpeed, label = "|speed|")
    plt.plot(angle, xSpeed, label = "x.speed")
    plt.plot(angle, ySpeed, label = "y.speed")
    plt.plot(angle, motor1Speed, label = "motor1.speed")
    plt.plot(angle, motor2Speed, label = "motor2.speed")
    plt.plot(angle, motor3Speed, label = "motor3.speed")
    plt.legend()
    plt.grid()
    plt.show()