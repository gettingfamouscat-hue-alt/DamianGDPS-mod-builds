This is a 2.2081 Mac crash candidate based on DDimaa15/PlatformerSaves-2.2081.
It replaces a write to a hard-coded Geometry Dash address with the
GameObject::resetMID() binding, which supports Windows, Intel Mac, and Apple
Silicon Mac. The save format and mod ID are unchanged. Version is v1.2.2.

Source: https://github.com/DDimaa15/PlatformerSaves-2.2081
Dependency: https://github.com/0x5abe/PersistenceAPI
Both projects are GPL-3.0. This candidate is not publicly packaged for players
until it has been tested on Mac.
