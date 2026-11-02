import sys, time, serial

name, dauer = sys.argv[1], float(sys.argv[2])
ser = serial.Serial("/dev/cu.usbmodem1101", 9600, timeout=1)
t0 = time.time()
with open(name + ".csv", "w") as f:
    f.write("t,Tref,Tntc,R_kOhm\n")
    while time.time() - t0 < dauer:
        line = ser.readline().decode(errors="ignore").strip()
        try:
            Tref, Tntc, R = [float(p.split(":")[1]) for p in line.split(",")]
        except (ValueError, IndexError):
            continue
        t = time.time() - t0
        f.write(f"{t:.1f},{Tref},{Tntc},{R}\n")
        print(f"{t:6.1f} s   Tref = {Tref:.2f}   Tntc = {Tntc:.2f}   R = {R:.2f} kOhm")
