# BLACKJACK General Information
## RULES
### General
- เกมจะต้องมีผู้เล่นทั้งหมด 2 คน ถึงจะสามารถทำการเริ่มเล่นได้
- ผู้เล่นจะต้องทำการรวมแต้มไพ่ของตนเองให้ได้มากกว่า Dealer จึงจะถือว่าชนะ
- ในเด็คสามารถมีไพ่ซ้ำได้ ซึ่งจะทำการ Random ให้แก่ผู้เล่น
- แต้มไพ่จะต้องไม่เกิน 21 หากแต้มของผู้เล่นเกิน 21 ไม่ว่ากรณีใดๆจะถือว่าผู้เล่นแพ้ทันที
- K Q J จะมีค่าเท่ากับ 10, ACE สามารถมีค่าได้ทั้ง 1 หรือ 11
- ผู้เล่นสามารถเลือก Hit เพื่อทำการจั่วไพ่เพิ่ม หรือเลือก Stand เพื่อทำการรอ Dealer เปิดไพ่ ซึ่งหากผู้เล่นมีไพ่ทั้งหมด 5 ใบแล้วจะไม่สามารถ Hit ต่อได้ และจะทำการ Stand โดยอัตโนมัติ
- หากผู้เล่นชนะจะได้ credit เท่ากับ 2 เท่าของ credit ที่ได้ทำการ bet ไป
### Betting
เงื่อนไขในการ bet
- bet เป็นหน่วย 100 credit (100, 200, 300, etc.) ซึ่ง 100 credit = 10 บาท
- สามารถ bet ได้แค่จำนวนเงินที่ player ได้ทำการเพิ่ม ซึ่ง สามารถเป็นได้มากสุด 1000 credit
- สามารถ bet ได้ 0 credit แต่จะไม่สามารถได้รางวัลจากการชนะเกมเช่นกัน

# Files
## Player
### player.ino
- เป็นไฟล์คำสั่งจัดการส่วนของ Player ที่มีการรับและส่งสัญญาณจากเครื่องของ Dealer  และ  Creditor คือ การส่งข้อมูลสถานะต่างๆ เช่น การเตรียมพร้อม การ bet ให้กับ Dealer และ การรับข้อมูลผลแพ้ชนะจาก Dealer

* การส่งข้อมูลต้องการนำเหรียญออก ไปที่ Creditor และ
การรับข้อมูลจากการหยอดเหรียญเข้าที่ Creditor
* รับคำสั่งจากผู้เล่นผ่านการกดปุ่มเพื่อเล่นเกม และใช้ rotary encoder ในการหมุนเพื่อ bet 
- แสดงผลสถานะต่างๆของผู้เล่น ผ่านจอ  OLED


## Dealer
### dealer.ino
- เป็นไฟล์คำสั่งจัดการส่วนของ Dealer ที่มีการรับและส่งสัญญาณจากเครื่องของ Player คือ การส่งข้อมูลการแจกไพ่ ส่งข้อมูลการตัดสินเกม ส่งข้อมูลการแสดงผลสถานะเกม เช่น การรอผู้เล่นอีกคนนึง  และการรับคำสั่งจากผู้เล่น 
* แสดงผลสถานะต่างๆของ Dealer ผ่านจอ  OLED

## Creditor
### creditor.ino
- เป็นไฟล์คำสั่งจัดการส่วนของเครื่องหยอดเหรียญ ที่มีการรับและส่งสัญญาณจากเครื่อง player โดยมีระบบการนับเหรียญที่ผู้เล่นเเป็นคนหยอด และ มีการคืนเหรียญเมื่อผู้เล่นต้องการ
-  แสดงข้อมูลเหรียญภายในกล่อง ผ่านจอ  OLED
-  ควบคุมการทำงานของ servo เพื่อทำการคืนเหรียญ

## Card
### card_printer.ino
- เป็นไฟล์ฟังก์ชันสำหรับเรียกใช้เรียกรูปไพ่และลายไพ่บนหน้าจอของ Player และ Dealer
### card_printer.h
- เป็นไฟล์ header สำหรับแสดงรูปไพ่ของ Player และ Dealer
### card_icon.h
- เป็นไฟล์ header สำหรับแสดงลายไพ่

# คณะผู้จัดทำ
- 6410500297 นางสาวภัทรวรรณ ศิริประเสริฐสุข
- 6410503971 นายกิตติชัย ตั้งปรัชญาวุธ
- 6410504268 นายรัชชานนท์ ทวีพงศธร
- 6410504306 นายวัลลภ วิวิธวรรัตน์
- 6410504390 นายอธิชา แพรไพรจิตรกุล
  
# ภาควิชาวิศวกรรมคอมพิวเตอร์ คณะวิศวกรรมศาสตร์ มหาวิทยาลัยเกษตรศาสตร์