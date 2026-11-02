import sys, time, serial

name = sys.argv[1]
ser = serial.Serial("/dev/cu.usbmodem1101", 9600, timeout=1)
f = None
while True:
    line = ser.readline().decode(errors="ignore").strip()
    if line == "ENDE":
        break
    if line.startswith("LAUF"):                      # neuer Lauf -> neue Datei
        if f:
            f.close()
        nr, ki = line.split()[1], line.split("Ki=")[1]
        fn = f"{name}_{nr}_Ki{ki}.csv"
        f = open(fn, "w")
        f.write("t,T,Tset,duty\n")
        t0 = time.time()
        print(f"==> {fn}")
        continue
    if f is None:
        continue
    try:
        T, Tset, luefter = [float(p.split(":")[1]) for p in line.split(",")]
    except (ValueError, IndexError):
        continue
    t = time.time() - t0
    f.write(f"{t:.1f},{T},{Tset},{luefter * 10:.0f}\n")
    f.flush()
    phase = "Aufheizen" if Tset == 0 else "Regelung"
    print(f"{t:6.1f} s   {phase:9}   T = {T:.2f}   duty = {luefter * 10:.0f}")
if f:
    f.close()
print("FERTIG")
