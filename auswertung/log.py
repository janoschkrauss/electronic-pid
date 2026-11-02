import sys, time, serial

name, dauer = sys.argv[1], float(sys.argv[2])
ser = serial.Serial("/dev/cu.usbmodem1101", 9600, timeout=1)
t0 = time.time()
with open(name + ".csv", "w") as f:
    f.write("t,T,Tset,duty\n")
    while time.time() - t0 < dauer:
        line = ser.readline().decode(errors="ignore").strip()
        try:
            T, Tset, luefter = [float(p.split(":")[1]) for p in line.split(",")]
        except ValueError:
            continue
        t = time.time() - t0
        f.write(f"{t:.1f},{T},{Tset},{luefter * 10:.0f}\n")
        print(f"{t:6.1f} s   T = {T:.2f}   duty = {luefter * 10:.0f}")
