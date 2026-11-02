import sys, time, serial

name = sys.argv[1]
phasen = {1: "Scan", 2: "kalt", 3: "Regel unten", 4: "heiss", 5: "Regel oben", 6: "Gleichgew."}
ser = serial.Serial("/dev/cu.usbmodem1101", 9600, timeout=3)
t0 = time.time()
with open(name + ".csv", "w") as f:
    f.write("t,phase,duty,rpm,T,Thoch\n")
    while True:
        line = ser.readline().decode(errors="ignore").strip()
        if line == "ENDE":
            break
        try:
            P, duty, rpm, T, Th = [float(p.split(":")[1]) for p in line.split(",")]
        except (ValueError, IndexError):
            continue
        t = time.time() - t0
        f.write(f"{t:.1f},{P:.0f},{duty:.0f},{rpm:.0f},{T},{Th}\n")
        f.flush()
        print(f"{t:7.1f} s  {phasen.get(int(P), '?'):11}  duty = {duty:3.0f} %  rpm = {rpm:5.0f}  T = {T:.2f}")
print("fertig")
