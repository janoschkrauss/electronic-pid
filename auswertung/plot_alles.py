import sys
import pandas as pd
import matplotlib.pyplot as plt

fn = sys.argv[1]
d = pd.read_csv(fn)
regel = d[d["Tset"] > 0]
t_start = regel["t"].iloc[0]

fig, (a1, a2) = plt.subplots(2, 1, sharex=True, figsize=(9, 6), height_ratios=[3, 1])
for a in (a1, a2):
    a.axvspan(d["t"].iloc[0], t_start, color="0.9", label="Aufheizen")
a1.plot(d["t"], d["T"], label=r"$T$")
a1.plot(regel["t"], regel["Tset"], "k--", label=r"$T_\mathrm{set}$")
a2.plot(d["t"], d["duty"] / 2.55)
a1.set_ylabel(r"$T$ [$^\circ$C]")
a1.legend()
a2.set_ylabel("Luefter [%]")
a2.set_xlabel(r"$t$ [s]")
fig.tight_layout()
fig.savefig(fn.replace(".csv", "_alles.png"), dpi=150)
plt.show()
