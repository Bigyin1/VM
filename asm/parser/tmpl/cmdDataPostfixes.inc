
#ifndef DATA_POSTFIX

#error "No DATA_POSTFIX macro"

#endif

DATA_POSTFIX(w, DataWord, NotSignExtended)

DATA_POSTFIX(s, DataWord, SignExtended)

DATA_POSTFIX(ws, DataWord, SignExtended)

DATA_POSTFIX(b, DataByte, NotSignExtended)

DATA_POSTFIX(bs, DataByte, SignExtended)

DATA_POSTFIX(db, DataDByte, NotSignExtended)

DATA_POSTFIX(dbs, DataDByte, SignExtended)

DATA_POSTFIX(hw, DataHalfWord, NotSignExtended)

DATA_POSTFIX(hws, DataHalfWord, SignExtended)
