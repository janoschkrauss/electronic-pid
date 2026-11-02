import sys
import pandas as pd
import matplotlib.pyplot as plt

fig, (a1, a2) = plt.subplots(2, 1, sharex=True, figsize=(8, 6), height_ratios=[3, 1])
for fn in sys.argv[1:]:
    d = pd.read_csv(fn)
    d = d[d["Tset"] > 0]                 # Aufheizphase (Tset = 0) weglassen
    t = d["t"] - d["t"].iloc[0]          # t = 0 beim Start des Reglers
    a1.plot(t, d["T"], label=fn.replace(".csv", ""))
    a2.plot(t, d["duty"] / 2.55)
a1.axhline(d["Tset"].iloc[0], ls="--", c="k", label=r"$T_\mathrm{set}$")
a1.set_ylabel(r"$T$ [$^\circ$C]")
a1.legend()
a2.set_ylabel("Luefter [%]")
a2.set_xlabel(r"$t$ [s]")
fig.tight_layout()
fig.savefig("task27.png", dpi=150)
plt.show()
