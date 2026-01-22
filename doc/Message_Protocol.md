# Message Protocol

The message protocol is well explored in this [document](https://arxiv.org/pdf/1304.1672) in case you would like to get more information.

## Initial Handshake

To start a Quick Race, the user must connect to PORT 3001 and send a request similar to the following one:

```
"SCR(init -1.0 1.0 1.0 0.0 \"Driver\")"
```

Where 'Driver' can be changed to whatever name we prefer.

## Messages from Server

The client receives a string from the server:

```
Message from server: 
    (angle -0.0072934)
    (curLapTime 3.678)
    (damage 0)
    (distFromStart 5759.1)
    (distRaced 0)
    (fuel 94)
    (gear 0)
    (lastLapTime 0)
    (opponents 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200)
    (racePos 1)
    (rpm 942.478)
    (speedX -0.00908379)
    (speedY 0.010119)
    (speedZ 0.000112499)
    (track 7.33362 7.43677 7.78138 8.42638 9.49934 11.2719 14.3745 20.5828 36.8449 193.783 23.5038 11.0548 7.45584 5.76452 4.82012 4.25338 3.91284 3.72803 3.66663)
    (trackPos -0.333363)
    (wheelSpinVel 0.907905 -0.614381 4.26186 -3.20328)
    (z 0.345255)
    (focus -1 -1 -1 -1 -1)
    (x 602.857)
    (y 1167.06)
    (roll -1.14492e-06)
    (pitch 0.00539013)
    (yaw 0.0408844)
    (speedGlobalX -0.00263617)
    (speedGlobalY 0.00270522)
```

## Messages from Client

The client must send a string to the server:

```
Message from client:
    (accel <float>)
    (brake <float>)
    (clutch <float>)
    (gear <int>)
    (steer <float>)
    (focus <float>)
    (meta <bool>)
```

---

# Sensors

trackSensAngle[0] -90
trackSensAngle[1] -80
trackSensAngle[2] -70
trackSensAngle[3] -60
trackSensAngle[4] -50
trackSensAngle[5] -40
trackSensAngle[6] -30
trackSensAngle[7] -20
trackSensAngle[8] -10
trackSensAngle[9]  0
trackSensAngle[10] 10
trackSensAngle[11] 20
trackSensAngle[12] 30
trackSensAngle[13] 40
trackSensAngle[14] 50
trackSensAngle[15] 60
trackSensAngle[16] 70
trackSensAngle[17] 80
trackSensAngle[18] 90
