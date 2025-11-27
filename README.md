# 🚨 Vehicle Accident Alert & Tracking System

A microcontroller-based system for **accident detection**, **location tracking**, and **automatic emergency alerts**. The device identifies collisions, rollovers, and fires, determines driver-carelessness level, and sends real-time GPS coordinates to emergency services via GSM.  
Full system description referenced from project documentation.

---

## 📌 Features
- **Accident Detection:** Collision, rollover, and fire sensing  
- **Driver Carelessness Assessment:** Based on activation order  
- **GPS Tracking:** Real-time location via NEO-6M  
- **GSM Alerts:** SMS sent to emergency units with:
  - Accident type  
  - Google Maps link  
  - Driver-carelessness note  
- **Cancel Switch:** 5–15 second safety window  
- **LCD Display:** Status, alerts, and countdown  
- **Kalman Filtering:** IMU smoothing for accurate rollover detection

---

## 🛠️ Hardware Components
- Arduino Nano (ATmega328P)  
- MPU6050 IMU (Accelerometer + Gyroscope)  
- Vibration/Knock Sensor  
- Flame Sensor  
- NEO-6M GPS Module  
- SIM800C GSM Module  
- LM2596 Buck Converter  
- 16×2 LCD (I2C)  
- Buzzer, LEDs, Push Button  

---

## 🔧 System Architecture
1. Detect accident via IMU, knock, and flame sensors  
2. Identify accident type  
3. Fetch location from GPS  
4. Countdown (5–15s) displayed on LCD  
5. User may cancel non-critical alerts  
6. GSM sends emergency SMS with:
   - Accident category  
   - Carelessness remark  
   - GPS Google Maps link  
7. Audible/visual alerts through buzzer + LEDs  

---

## 🧩 Accident Severity & Delay Logic
| Collision | Rollover | Fire | Delay |
|----------|----------|------|--------|
| Yes | No | No | 15s |
| No | Yes | No | 15s |
| Yes | No | Yes | 10s |
| No | Yes | Yes | 10s |
| Yes | Yes | No | 5s |
| Yes | Yes | Yes | 5s |

---

## 🧪 Results Summary
- **Accident detection accuracy:** 100%  
- **GPS tracking accuracy:** 87%  
- **SMS delivery success:** 93%  
- Tested across multiple real-world road conditions

---

## 📁 Suggested Project Structure
