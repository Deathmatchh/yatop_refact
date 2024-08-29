
# Yatops - Yet another Tales of Pirates Source
## Built for relentless performance & stability™️!

## Yatops Features
This may not be a full list of features

Performance & Stability:
- Fixed numerous memory leaks in server-side.
- Added Bcrypt hashing for passwords and RSA-AES packet encryption
- Replaced SQL Queries with SQL Stored Procedures (no SQL injection anymore)
- Replaced CaLua with LuaJIT in server-side.
- Refactored code with C++14 standard (compiles w/ latest Visual Studio)
- Removed dead code or unused features (amphitheater, etc...)
- Server-side x64 architecture
- UI Files, UI Images and tables encryption (obfuscated w/ obfy)

New available (to be ported) client features:
- Monster info/monster drop info: when right-clicking mobs, a dropdown menu opens showcasing these 2 new UI.
- Guild Bank: may be deprecated.
- Guild Bank log: full log database of bank operations (who took what).
- More character slots: new character slots.
- FPS Limitation: sliding bar to select framerate limit (30 to 144), without animation issues.
- CO inventory: contains the character 3D preview.
- Daily Rewards & Daily Progression: UI with server database for server daily rewards. Also has a daily character progression that is not linked to the server rewards.
- Expiration & tradeability: new item properties. Expiration wont let you equip/use the item if it's expired, and tradeability won't let you drop it/trade it.
