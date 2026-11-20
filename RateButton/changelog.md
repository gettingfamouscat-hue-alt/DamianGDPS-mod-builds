# 1.1.7
- List cells (Impossible / Very Easy): **face → right**, **VIEW → left** (was the other way)
- Access 1 (Mod) popup title is **Mod: Send Mod** (still send-to-queue, not rate)
- Re-send: `!send` gets a nonce; server upserts suggest row so Sent tab bumps again
- Custom faces no longer gated on `isOnGdps()` (Mac URL offsets were hiding VE/Impossible)
- Hardened GDPS URL detection for Mac offsets

# 1.1.6
- Very Easy: redraw featured/epic/legendary/mythic **fire coins** behind the custom face (hiding stock `GJDifficultySprite` was killing colored fire)
- Very Easy: nudge `stars-label` / `stars-icon` down so the stacked VERY/EASY caption doesn't cover the star count (list + info)
- Force custom face + captions to full white (no inherited grey)

# 1.1.5
- Switch pickers (Select Demon / Select Difficulty) show difficulty faces beside each option
- Very Easy caption stack is **VERY** over **EASY** (list / info / share)
- Impossible Demon list cells: larger VIEW / face / name shifts so titles aren't cramped

# 1.1.4
- Fix missing yellow Rate Button when `requestUserAccess` arrives after open (always re-request; longer poll)
- Only hide stock mod-rate when our button is actually available (access>=1)
- Warm mod access on MenuLayer for GDPS so Send/Rate is ready sooner
- Very Easy / Impossible faces + search filters stay GDPS-wide (not mod-gated)

# 1.1.3
- Revert online list widen (back to stock 356 width)
- **Impossible Demon** list cells only: nudge VIEW + level-thumbnail preview, hide song/stats clutter, widen name label — other difficulties stay stock
- **Very Easy** face caption stacks **EASY** over **VERY** (list cell + level info + share request)

# 1.1.2
- Widen online level browser list (+36px) and level-name label max width so longer titles fit
- Search filters: **Impossible Demon** in the demon **+** subtype popup
- Search filters: **+** on Easy → Easy / Very Easy (same idea as demon subtypes)
- Search is **server-side** via `demonFilter=6` / `easyFilter` on `getGJLevels`

# 1.1.1
- Fix Very Easy / Impossible Demon face scale (match stock difficulty size; no more tiny PNGs)
- Show custom faces + **VERY EASY** / **IMPOSSIBLE** captions on **LevelCell** list cards and LevelInfoLayer
- Elder/mod: hide stock **mod-rate-button** so only the yellow Rate Button remains
- Share Level UI: **Switch** under the face (demon subtypes / Easy↔Very Easy), custom faces at correct size
- Rename **REQUEST STARS** → **DIFFICULTY REQUESTED**; Level Info **Stars Requested** → **Difficulty Requested: (name)**

# 1.1.0
- Yellow/gold mod star rate button (removed cyan tint); Elder Damian gets it too
- Elder popup title is **Mod: Rate Level** (send-only: **Mod: Send Level**)
- **Switch** under the face: at 10★ pick Easy/Medium/Hard/Insane/Extreme/Impossible Demon; at 2★ pick Easy vs Very Easy
- **Very Easy** custom face; server accepts `very-easy` / `veryeasy` — stored as Easy (`starDifficulty=10`) + `starDemonDiff=1` while `starDemon=0` (documented in Utils.hpp)
- Keep stars award toggle, fire/coins, Impossible Demon (`starDemonDiff=7`), versus untouched

# 1.0.9
- Add **Impossible Demon** after Extreme in the 10★ face cycle (custom face sprite)
- Rates send `impossible-demon` (`starDemonDiff=7`); LevelInfoLayer swaps the face in-game on GDPS

# 1.0.5
- Fix missing Rate Button when mod access arrives after the level page opens
- Re-check GDPS URL live; poll briefly for elder/mod access; fall back to right-side menu

# 1.0.6
- Toggle top-left star: **STARS** mode (normal star rate) vs **FACE** mode (difficulty face only, 0 stars)
- In FACE mode click the face / arrows to cycle Easy→…→Extreme Demon, Submit sets face without stars

# 1.0.7
- Top-left **star logo toggle**: on = award stars, off (dim + X) = 0 stars but still sets face / fire / coins
- Pick difficulty with the star count as usual; toggle only controls whether stars are given

# 1.0.8
- Move star award toggle above the star count so it isn’t next to the popup close (X)

# 1.0.4
- Normal Mod gets the Rate Button with Send (not rate). Elder gets full Submit/rate UI
- Nobody sees the button until Damian assigns them Moderator or Elder Mod

# 1.0.3
- Rate Button only appears if the GDPS granted you mod/elder (via requestUserAccess)
- Vanilla thumbs / demon rate buttons hidden on GDPS — use Rate Button instead
- After Submit / Send, the level page re-downloads so difficulty updates without leaving

# 1.0.2
- Click the demon face at 10 stars to cycle Easy / Medium / Hard / Insane / Extreme Demon
- Rates send `easy-demon` … `extreme-demon` so the GDPS stores `starDemonDiff` correctly

# 1.0.1
- Fixed a button positioning issue

# 1.0.0
- Added toggleable buttons to set the level for daily/weekly & to only send the level
- Improved performance (very slightly) & clarified the read me and some popups' text
- Added an ID to the difficulty face sprite
- Mod now disables itself when not on a GDPS
