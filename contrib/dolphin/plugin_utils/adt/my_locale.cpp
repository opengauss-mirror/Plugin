/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 */

#include <cstring>
#include "plugin_utils/my_locale.h"

#ifdef DOLPHIN
/***** LOCALE BEGIN ar_AE: Arabic - United Arab Emirates *****/
static char *month_names_ar_AE[] = {
    "يناير", "فبراير", "مارس",   "أبريل",  "مايو",   "يونيو", "يوليو",
    "أغسطس", "سبتمبر", "أكتوبر", "نوفمبر", "ديسمبر"};
static char *ab_month_names_ar_AE[] = {
    "ينا", "فبر", "مار", "أبر", "ماي", "يون", "يول",
    "أغس", "سبت", "أكت", "نوف", "ديس"};
static char *day_names_ar_AE[] = {
    "الاثنين", "الثلاثاء", "الأربعاء", "الخميس",
    "الجمعة",  "السبت ",   "الأحد"};
static char *ab_day_names_ar_AE[] = {"ن", "ث", "ر", "خ",
                                                      "ج", "س", "ح"};
/***** LOCALE END ar_AE *****/

/***** LOCALE BEGIN ar_BH: Arabic - Bahrain *****/
static char *month_names_ar_BH[] = {
    "يناير", "فبراير", "مارس",   "أبريل",  "مايو",   "يونيو", "يوليو",
    "أغسطس", "سبتمبر", "أكتوبر", "نوفمبر", "ديسمبر"};
static char *ab_month_names_ar_BH[] = {
    "ينا", "فبر", "مار", "أبر", "ماي", "يون", "يول",
    "أغس", "سبت", "أكت", "نوف", "ديس"};
static char *day_names_ar_BH[] = {
    "الاثنين", "الثلاثاء", "الأربعاء", "الخميس",
    "الجمعة",  "السبت",    "الأحد"};
static char *ab_day_names_ar_BH[] = {"ن", "ث", "ر", "خ",
                                                      "ج", "س", "ح"};
/***** LOCALE END ar_BH *****/

/***** LOCALE BEGIN ar_JO: Arabic - Jordan *****/
static char *month_names_ar_JO[] = {
    "كانون الثاني", "شباط",        "آذار", "نيسان", "نوار",
    "حزيران",       "تموز",        "آب",   "أيلول", "تشرين الأول",
    "تشرين الثاني", "كانون الأول"};
static char *ab_month_names_ar_JO[] = {
    "كانون الثاني", "شباط",        "آذار", "نيسان", "نوار",
    "حزيران",       "تموز",        "آب",   "أيلول", "تشرين الأول",
    "تشرين الثاني", "كانون الأول"};
static char *day_names_ar_JO[] = {
    "الاثنين", "الثلاثاء", "الأربعاء", "الخميس",
    "الجمعة",  "السبت",    "الأحد"};
static char *ab_day_names_ar_JO[] = {
    "الاثنين", "الثلاثاء", "الأربعاء", "الخميس",
    "الجمعة",  "السبت",    "الأحد"};
/***** LOCALE END ar_JO *****/

/***** LOCALE BEGIN ar_SA: Arabic - Saudi Arabia *****/
static char *month_names_ar_SA[] = {
    "كانون الثاني", "شباط",        "آذار", "نيسـان", "أيار",
    "حزيران",       "تـمـوز",      "آب",   "أيلول",  "تشرين الأول",
    "تشرين الثاني", "كانون الأول"};
static char *ab_month_names_ar_SA[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
    "Aug", "Sep", "Oct", "Nov", "Dec"};
static char *day_names_ar_SA[] = {
    "الإثنين", "الثلاثاء", "الأربعاء", "الخميس",
    "الجمعـة", "السبت",    "الأحد"};
static char *ab_day_names_ar_SA[] = {
    "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
/***** LOCALE END ar_SA *****/

/***** LOCALE BEGIN ar_SY: Arabic - Syria *****/
static char *month_names_ar_SY[] = {
    "كانون الثاني", "شباط",        "آذار", "نيسان", "نواران",
    "حزير",         "تموز",        "آب",   "أيلول", "تشرين الأول",
    "تشرين الثاني", "كانون الأول"};
static char *ab_month_names_ar_SY[] = {
    "كانون الثاني", "شباط",        "آذار", "نيسان", "نوار",
    "حزيران",       "تموز",        "آب",   "أيلول", "تشرين الأول",
    "تشرين الثاني", "كانون الأول"};
static char *day_names_ar_SY[] = {
    "الاثنين", "الثلاثاء", "الأربعاء", "الخميس",
    "الجمعة",  "السبت",    "الأحد"};
static char *ab_day_names_ar_SY[] = {
    "الاثنين", "الثلاثاء", "الأربعاء", "الخميس",
    "الجمعة",  "السبت",    "الأحد"};
/***** LOCALE END ar_SY *****/

/***** LOCALE BEGIN be_BY: Belarusian - Belarus *****/
static char *month_names_be_BY[] = {
    "Студзень", "Люты",    "Сакавік", "Красавік", "Травень",
    "Чэрвень",  "Ліпень",  "Жнівень", "Верасень", "Кастрычнік",
    "Лістапад", "Снежань"};
static char *ab_month_names_be_BY[] = {
    "Стд", "Лют", "Сак", "Крс", "Тра", "Чэр", "Ліп",
    "Жнв", "Врс", "Кст", "Ліс", "Снж"};
static char *day_names_be_BY[] = {
    "Панядзелак", "Аўторак", "Серада",  "Чацвер",
    "Пятніца",    "Субота",  "Нядзеля"};
static char *ab_day_names_be_BY[] = {
    "Пан", "Аўт", "Срд", "Чцв", "Пят", "Суб", "Няд"};
/***** LOCALE END be_BY *****/

/***** LOCALE BEGIN bg_BG: Bulgarian - Bulgaria *****/
static char *month_names_bg_BG[] = {
    "януари", "февруари",  "март",     "април",   "май",      "юни", "юли",
    "август", "септември", "октомври", "ноември", "декември"};
static char *ab_month_names_bg_BG[] = {
    "яну", "фев", "мар", "апр", "май", "юни", "юли",
    "авг", "сеп", "окт", "ное", "дек"};
static char *day_names_bg_BG[] = {
    "понеделник", "вторник", "сряда",  "четвъртък",
    "петък",      "събота",  "неделя"};
static char *ab_day_names_bg_BG[] = {"пн", "вт", "ср", "чт",
                                                      "пт", "сб", "нд"};
/***** LOCALE END bg_BG *****/

/***** LOCALE BEGIN ca_ES: Catalan - Catalan *****/
static char *month_names_ca_ES[] = {
    "gener", "febrer",   "març",    "abril",    "maig",     "juny", "juliol",
    "agost", "setembre", "octubre", "novembre", "desembre"};
static char *ab_month_names_ca_ES[] = {
    "gen", "feb", "mar", "abr", "mai", "jun", "jul",
    "ago", "set", "oct", "nov", "des"};
static char *day_names_ca_ES[] = {
    "dilluns",   "dimarts",  "dimecres", "dijous",
    "divendres", "dissabte", "diumenge"};
static char *ab_day_names_ca_ES[] = {"dl", "dt", "dc", "dj",
                                                      "dv", "ds", "dg"};
/***** LOCALE END ca_ES *****/

/***** LOCALE BEGIN cs_CZ: Czech - Czech Republic *****/
static char *month_names_cs_CZ[] = {
    "leden", "únor", "březen", "duben",    "květen",   "červen", "červenec",
    "srpen", "září", "říjen",  "listopad", "prosinec"};
static char *ab_month_names_cs_CZ[] = {
    "led", "úno", "bře", "dub", "kvě", "čen", "čec",
    "srp", "zář", "říj", "lis", "pro"};
static char *day_names_cs_CZ[] = {"Pondělí", "Úterý", "Středa",
                                                   "Čtvrtek", "Pátek", "Sobota",
                                                   "Neděle"};
static char *ab_day_names_cs_CZ[] = {"Po", "Út", "St", "Čt",
                                                      "Pá", "So", "Ne"};
/***** LOCALE END cs_CZ *****/

/***** LOCALE BEGIN da_DK: Danish - Denmark *****/
static char *month_names_da_DK[] = {
    "januar", "februar",   "marts",   "april",    "maj",      "juni", "juli",
    "august", "september", "oktober", "november", "december"};
static char *ab_month_names_da_DK[] = {
    "jan", "feb", "mar", "apr", "maj", "jun", "jul",
    "aug", "sep", "okt", "nov", "dec"};
static char *day_names_da_DK[] = {
    "mandag", "tirsdag", "onsdag", "torsdag",
    "fredag", "lørdag",  "søndag"};
static char *ab_day_names_da_DK[] = {
    "man", "tir", "ons", "tor", "fre", "lør", "søn"};
/***** LOCALE END da_DK *****/

/***** LOCALE BEGIN de_AT: German - Austria *****/
static char *month_names_de_AT[] = {
    "Jänner", "Feber",     "März",    "April",    "Mai",      "Juni", "Juli",
    "August", "September", "Oktober", "November", "Dezember"};
static char *ab_month_names_de_AT[] = {
    "Jän", "Feb", "Mär", "Apr", "Mai", "Jun", "Jul",
    "Aug", "Sep", "Okt", "Nov", "Dez"};
static char *day_names_de_AT[] = {
    "Montag",  "Dienstag", "Mittwoch", "Donnerstag",
    "Freitag", "Samstag",  "Sonntag"};
static char *ab_day_names_de_AT[] = {
    "Mon", "Die", "Mit", "Don", "Fre", "Sam", "Son"};
/***** LOCALE END de_AT *****/

/***** LOCALE BEGIN de_DE: German - Germany *****/
static char *month_names_de_DE[] = {
    "Januar", "Februar",   "März",    "April",    "Mai",      "Juni", "Juli",
    "August", "September", "Oktober", "November", "Dezember"};
static char *ab_month_names_de_DE[] = {
    "Jan", "Feb", "Mär", "Apr", "Mai", "Jun", "Jul",
    "Aug", "Sep", "Okt", "Nov", "Dez"};
static char *day_names_de_DE[] = {
    "Montag",  "Dienstag", "Mittwoch", "Donnerstag",
    "Freitag", "Samstag",  "Sonntag"};
static char *ab_day_names_de_DE[] = {"Mo", "Di", "Mi", "Do",
                                                      "Fr", "Sa", "So"};
/***** LOCALE END de_DE *****/

/***** LOCALE BEGIN en_US: English - United States *****/
static char *month_names_en_US[] = {
    "January", "February",  "March",   "April",    "May",      "June", "July",
    "August",  "September", "October", "November", "December"};
static char *ab_month_names_en_US[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
    "Aug", "Sep", "Oct", "Nov", "Dec"};
static char *day_names_en_US[] = {
    "Monday", "Tuesday",  "Wednesday", "Thursday",
    "Friday", "Saturday", "Sunday"};
static char *ab_day_names_en_US[] = {
    "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
/***** LOCALE END en_US *****/

/***** LOCALE BEGIN es_ES: Spanish - Spain *****/
static char *month_names_es_ES[] = {
    "enero",     "febrero",   "marzo",  "abril",      "mayo",
    "junio",     "julio",     "agosto", "septiembre", "octubre",
    "noviembre", "diciembre"};
static char *ab_month_names_es_ES[] = {
    "ene", "feb", "mar", "abr", "may", "jun", "jul",
    "ago", "sep", "oct", "nov", "dic"};
static char *day_names_es_ES[] = {
    "lunes",   "martes", "miércoles", "jueves",
    "viernes", "sábado", "domingo"};
static char *ab_day_names_es_ES[] = {
    "lun", "mar", "mié", "jue", "vie", "sáb", "dom"};
/***** LOCALE END es_ES *****/

/***** LOCALE BEGIN et_EE: Estonian - Estonia *****/
static char *month_names_et_EE[] = {
    "jaanuar",  "veebruar",  "märts",  "aprill",    "mai",
    "juuni",    "juuli",     "august", "september", "oktoober",
    "november", "detsember"};
static char *ab_month_names_et_EE[] = {
    "jaan ", "veebr", "märts", "apr  ", "mai  ", "juuni", "juuli",
    "aug  ", "sept ", "okt  ", "nov  ", "dets "};
static char *day_names_et_EE[] = {
    "esmaspäev", "teisipäev", "kolmapäev", "neljapäev",
    "reede",     "laupäev",   "pühapäev"};
static char *ab_day_names_et_EE[] = {"E", "T", "K", "N",
                                                      "R", "L", "P"};
/***** LOCALE END et_EE *****/

/***** LOCALE BEGIN eu_ES: Basque - Basque *****/
static char *month_names_eu_ES[] = {
    "urtarrila", "otsaila", "martxoa", "apirila", "maiatza",
    "ekaina",    "uztaila", "abuztua", "iraila",  "urria",
    "azaroa",    "abendua"};
static char *ab_month_names_eu_ES[] = {
    "urt", "ots", "mar", "api", "mai", "eka", "uzt",
    "abu", "ira", "urr", "aza", "abe"};
static char *day_names_eu_ES[] = {
    "astelehena", "asteartea", "asteazkena", "osteguna",
    "ostirala",   "larunbata", "igandea"};
static char *ab_day_names_eu_ES[] = {
    "al.", "ar.", "az.", "og.", "or.", "lr.", "ig."};
/***** LOCALE END eu_ES *****/

/***** LOCALE BEGIN fi_FI: Finnish - Finland *****/
static char *month_names_fi_FI[] = {
    "tammikuu",  "helmikuu", "maaliskuu", "huhtikuu", "toukokuu",
    "kesäkuu",   "heinäkuu", "elokuu",    "syyskuu",  "lokakuu",
    "marraskuu", "joulukuu"};
static char *ab_month_names_fi_FI[] = {
    "tammi ", "helmi ", "maalis", "huhti ", "touko ", "kesä  ", "heinä ",
    "elo   ", "syys  ", "loka  ", "marras", "joulu "};
static char *day_names_fi_FI[] = {
    "maanantai", "tiistai",  "keskiviikko", "torstai",
    "perjantai", "lauantai", "sunnuntai"};
static char *ab_day_names_fi_FI[] = {"ma", "ti", "ke", "to",
                                                      "pe", "la", "su"};
/***** LOCALE END fi_FI *****/


/***** LOCALE BEGIN fo_FO: Faroese - Faroe Islands *****/
static char *month_names_fo_FO[] = {
    "januar", "februar",   "mars",    "apríl",    "mai",      "juni", "juli",
    "august", "september", "oktober", "november", "desember"};
static char *ab_month_names_fo_FO[] = {
    "jan", "feb", "mar", "apr", "mai", "jun", "jul",
    "aug", "sep", "okt", "nov", "des"};
static char *day_names_fo_FO[] = {
    "mánadagur",    "týsdagur",    "mikudagur",  "hósdagur",
    "fríggjadagur", "leygardagur", "sunnudagur"};
static char *ab_day_names_fo_FO[] = {
    "mán", "týs", "mik", "hós", "frí", "ley", "sun"};
/***** LOCALE END fo_FO *****/

/***** LOCALE BEGIN fr_FR: French - France *****/
static char *month_names_fr_FR[] = {
    "janvier",  "février",  "mars", "avril",     "mai",
    "juin",     "juillet",  "août", "septembre", "octobre",
    "novembre", "décembre"};
static char *ab_month_names_fr_FR[] = {
    "jan", "fév", "mar", "avr", "mai", "jun", "jui",
    "aoû", "sep", "oct", "nov", "déc"};
static char *day_names_fr_FR[] = {
    "lundi",    "mardi",  "mercredi", "jeudi",
    "vendredi", "samedi", "dimanche"};
static char *ab_day_names_fr_FR[] = {
    "lun", "mar", "mer", "jeu", "ven", "sam", "dim"};
/***** LOCALE END fr_FR *****/

/***** LOCALE BEGIN gl_ES: Galician - Galician *****/
static char *month_names_gl_ES[] = {
    "Xaneiro", "Febreiro", "Marzo",   "Abril",    "Maio",     "Xuño", "Xullo",
    "Agosto",  "Setembro", "Outubro", "Novembro", "Decembro"};
static char *ab_month_names_gl_ES[] = {
    "Xan", "Feb", "Mar", "Abr", "Mai", "Xuñ", "Xul",
    "Ago", "Set", "Out", "Nov", "Dec"};
static char *day_names_gl_ES[] = {
    "Luns",   "Martes", "Mércores", "Xoves",
    "Venres", "Sábado", "Domingo"};
static char *ab_day_names_gl_ES[] = {
    "Lun", "Mar", "Mér", "Xov", "Ven", "Sáb", "Dom"};
/***** LOCALE END gl_ES *****/

/***** LOCALE BEGIN gu_IN: Gujarati - India *****/
static char *month_names_gu_IN[] = {
    "જાન્યુઆરી", "ફેબ્રુઆરી", "માર્ચ",   "એપ્રિલ", "મે",      "જુન", "જુલાઇ",
    "ઓગસ્ટ",    "સેપ્ટેમ્બર", "ઓક્ટોબર", "નવેમ્બર", "ડિસેમ્બર"};
static char *ab_month_names_gu_IN[] = {
    "જાન", "ફેબ",  "માર", "એપ્ર", "મે",   "જુન", "જુલ",
    "ઓગ",  "સેપ્ટ", "ઓક્ટ", "નોવ", "ડિસ"};
static char *day_names_gu_IN[] = {
    "સોમવાર", "મન્ગળવાર", "બુધવાર", "ગુરુવાર", "શુક્રવાર", "શનિવાર", "રવિવાર"};
static char *ab_day_names_gu_IN[] = {
    "સોમ", "મન્ગળ", "બુધ", "ગુરુ", "શુક્ર", "શનિ", "રવિ"};
/***** LOCALE END gu_IN *****/

/***** LOCALE BEGIN he_IL: Hebrew - Israel *****/
static char *month_names_he_IL[] = {
    "ינואר",  "פברואר", "מרץ",     "אפריל",  "מאי",   "יוני", "יולי",
    "אוגוסט", "ספטמבר", "אוקטובר", "נובמבר", "דצמבר"};
static char *ab_month_names_he_IL[] = {
    "ינו", "פבר", "מרץ", "אפר", "מאי", "יונ", "יול",
    "אוג", "ספט", "אוק", "נוב", "דצמ"};
static char *day_names_he_IL[] = {
    "שני", "שלישי", "רביעי", "חמישי", "שישי", "שבת", "ראשון"};
static char *ab_day_names_he_IL[] = {"ב'", "ג'", "ד'", "ה'",
                                                      "ו'", "ש'", "א'"};
/***** LOCALE END he_IL *****/

/***** LOCALE BEGIN hi_IN: Hindi - India *****/
static char *month_names_hi_IN[] = {
    "जनवरी", "फ़रवरी",  "मार्च",  "अप्रेल",  "मई",     "जून", "जुलाई",
    "अगस्त",  "सितम्बर", "अक्टूबर", "नवम्बर", "दिसम्बर"};
static char *ab_month_names_hi_IN[] = {
    "जनवरी", "फ़रवरी",  "मार्च",  "अप्रेल",  "मई",     "जून", "जुलाई",
    "अगस्त",  "सितम्बर", "अक्टूबर", "नवम्बर", "दिसम्बर"};
static char *day_names_hi_IN[] = {
    "सोमवार ", "मंगलवार ", "बुधवार ",  "गुरुवार ",
    "शुक्रवार ", "शनिवार ", "रविवार "};
static char *ab_day_names_hi_IN[] = {
    "सोम ", "मंगल ", "बुध ", "गुरु ", "शुक्र ", "शनि ", "रवि "};
/***** LOCALE END hi_IN *****/

/***** LOCALE BEGIN hr_HR: Croatian - Croatia *****/
static char *month_names_hr_HR[] = {
    "Siječanj", "Veljača",  "Ožujak",  "Travanj", "Svibanj",
    "Lipanj",   "Srpanj",   "Kolovoz", "Rujan",   "Listopad",
    "Studeni",  "Prosinac"};
static char *ab_month_names_hr_HR[] = {
    "Sij", "Vel", "Ožu", "Tra", "Svi", "Lip", "Srp",
    "Kol", "Ruj", "Lis", "Stu", "Pro"};
static char *day_names_hr_HR[] = {
    "Ponedjeljak", "Utorak", "Srijeda",  "Četvrtak",
    "Petak",       "Subota", "Nedjelja"};
static char *ab_day_names_hr_HR[] = {
    "Pon", "Uto", "Sri", "Čet", "Pet", "Sub", "Ned"};
/***** LOCALE END hr_HR *****/

/***** LOCALE BEGIN hu_HU: Hungarian - Hungary *****/
static char *month_names_hu_HU[] = {
    "január",   "február",  "március",   "április",    "május",
    "június",   "július",   "augusztus", "szeptember", "október",
    "november", "december"};
static char *ab_month_names_hu_HU[] = {
    "jan", "feb", "már", "ápr", "máj", "jún", "júl",
    "aug", "sze", "okt", "nov", "dec"};
static char *day_names_hu_HU[] = {
    "hétfő",  "kedd",    "szerda",   "csütörtök",
    "péntek", "szombat", "vasárnap"};
static char *ab_day_names_hu_HU[] = {"h", "k",   "sze", "cs",
                                                      "p", "szo", "v"};
/***** LOCALE END hu_HU *****/

/***** LOCALE BEGIN id_ID: Indonesian - Indonesia *****/
static char *month_names_id_ID[] = {
    "Januari", "Pebruari",  "Maret",   "April",    "Mei",      "Juni", "Juli",
    "Agustus", "September", "Oktober", "November", "Desember"};
static char *ab_month_names_id_ID[] = {
    "Jan", "Peb", "Mar", "Apr", "Mei", "Jun", "Jul",
    "Agu", "Sep", "Okt", "Nov", "Des"};
static char *day_names_id_ID[] = {
    "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu", "Minggu"};
static char *ab_day_names_id_ID[] = {
    "Sen", "Sel", "Rab", "Kam", "Jum", "Sab", "Min"};
/***** LOCALE END id_ID *****/

/***** LOCALE BEGIN is_IS: Icelandic - Iceland *****/
static char *month_names_is_IS[] = {
    "janúar", "febrúar",   "mars",    "apríl",    "maí",      "júní", "júlí",
    "ágúst",  "september", "október", "nóvember", "desember"};
static char *ab_month_names_is_IS[] = {
    "jan", "feb", "mar", "apr", "maí", "jún", "júl",
    "ágú", "sep", "okt", "nóv", "des"};
static char *day_names_is_IS[] = {
    "mánudagur",  "þriðjudagur", "miðvikudagur", "fimmtudagur",
    "föstudagur", "laugardagur", "sunnudagur"};
static char *ab_day_names_is_IS[] = {
    "mán", "þri", "mið", "fim", "fös", "lau", "sun"};
/***** LOCALE END is_IS *****/

/***** LOCALE BEGIN it_CH: Italian - Switzerland *****/
static char *month_names_it_CH[] = {
    "gennaio",  "febbraio", "marzo",  "aprile",    "maggio",
    "giugno",   "luglio",   "agosto", "settembre", "ottobre",
    "novembre", "dicembre"};
static char *ab_month_names_it_CH[] = {
    "gen", "feb", "mar", "apr", "mag", "giu", "lug",
    "ago", "set", "ott", "nov", "dic"};
static char *day_names_it_CH[] = {
    "lunedì",  "martedì", "mercoledì", "giovedì",
    "venerdì", "sabato",  "domenica"};
static char *ab_day_names_it_CH[] = {
    "lun", "mar", "mer", "gio", "ven", "sab", "dom"};
/***** LOCALE END it_CH *****/

/***** LOCALE BEGIN ja_JP: Japanese - Japan *****/
static char *month_names_ja_JP[] = {
    "1月", "2月", "3月",  "4月",  "5月",  "6月", "7月",
    "8月", "9月", "10月", "11月", "12月"};
static char *ab_month_names_ja_JP[] = {
    " 1月", " 2月", " 3月", " 4月", " 5月", " 6月", " 7月",
    " 8月", " 9月", "10月", "11月", "12月"};
static char *day_names_ja_JP[] = {"月曜日", "火曜日", "水曜日",
                                                   "木曜日", "金曜日", "土曜日",
                                                   "日曜日"};
static char *ab_day_names_ja_JP[] = {"月", "火", "水", "木",
                                                      "金", "土", "日"};
/***** LOCALE END ja_JP *****/

/***** LOCALE BEGIN ko_KR: Korean - Korea *****/
static char *month_names_ko_KR[] = {
    "일월", "이월", "삼월", "사월",   "오월",   "유월", "칠월",
    "팔월", "구월", "시월", "십일월", "십이월"};
static char *ab_month_names_ko_KR[] = {
    " 1월", " 2월", " 3월", " 4월", " 5월", " 6월", " 7월",
    " 8월", " 9월", "10월", "11월", "12월"};
static char *day_names_ko_KR[] = {"월요일", "화요일", "수요일",
                                                   "목요일", "금요일", "토요일",
                                                   "일요일"};
static char *ab_day_names_ko_KR[] = {"월", "화", "수", "목",
                                                      "금", "토", "일"};
/***** LOCALE END ko_KR *****/

/***** LOCALE BEGIN lt_LT: Lithuanian - Lithuania *****/
static char *month_names_lt_LT[] = {
    "sausio",    "vasario",  "kovo",      "balandžio", "gegužės",
    "birželio",  "liepos",   "rugpjūčio", "rugsėjo",   "spalio",
    "lapkričio", "gruodžio"};
static char *ab_month_names_lt_LT[] = {
    "Sau", "Vas", "Kov", "Bal", "Geg", "Bir", "Lie",
    "Rgp", "Rgs", "Spa", "Lap", "Grd"};
static char *day_names_lt_LT[] = {
    "Pirmadienis",  "Antradienis", "Trečiadienis", "Ketvirtadienis",
    "Penktadienis", "Šeštadienis", "Sekmadienis"};
static char *ab_day_names_lt_LT[] = {"Pr", "An", "Tr", "Kt",
                                                      "Pn", "Št", "Sk"};
/***** LOCALE END lt_LT *****/

/***** LOCALE BEGIN lv_LV: Latvian - Latvia *****/
static char *month_names_lv_LV[] = {
    "janvāris",  "februāris", "marts",   "aprīlis",    "maijs",
    "jūnijs",    "jūlijs",    "augusts", "septembris", "oktobris",
    "novembris", "decembris"};
static char *ab_month_names_lv_LV[] = {
    "jan", "feb", "mar", "apr", "mai", "jūn", "jūl",
    "aug", "sep", "okt", "nov", "dec"};
static char *day_names_lv_LV[] = {
    "pirmdiena",  "otrdiena",  "trešdiena", "ceturtdiena",
    "piektdiena", "sestdiena", "svētdiena"};
static char *ab_day_names_lv_LV[] = {"P ", "O ", "T ", "C ",
                                                      "Pk", "S ", "Sv"};
/***** LOCALE END lv_LV *****/

/***** LOCALE BEGIN mk_MK: Macedonian - FYROM *****/
static char *month_names_mk_MK[] = {
    "јануари", "февруари",  "март",     "април",   "мај",      "јуни", "јули",
    "август",  "септември", "октомври", "ноември", "декември"};
static char *ab_month_names_mk_MK[] = {
    "јан", "фев", "мар", "апр", "мај", "јун", "јул",
    "авг", "сеп", "окт", "ное", "дек"};
static char *day_names_mk_MK[] = {
    "понеделник", "вторник", "среда",  "четврток",
    "петок",      "сабота",  "недела"};
static char *ab_day_names_mk_MK[] = {
    "пон", "вто", "сре", "чет", "пет", "саб", "нед"};
/***** LOCALE END mk_MK *****/

/***** LOCALE BEGIN mn_MN: Mongolia - Mongolian *****/
static char *month_names_mn_MN[] = {"Нэгдүгээр сар",
                                                      "Хоёрдугаар сар",
                                                      "Гуравдугаар сар",
                                                      "Дөрөвдүгээр сар",
                                                      "Тавдугаар сар",
                                                      "Зургаадугар сар",
                                                      "Долоодугаар сар",
                                                      "Наймдугаар сар",
                                                      "Есдүгээр сар",
                                                      "Аравдугаар сар",
                                                      "Арваннэгдүгээр сар",
                                                      "Арванхоёрдгаар сар"};
static char *ab_month_names_mn_MN[] = {
    "1-р", "2-р", "3-р",  "4-р",  "5-р",  "6-р", "7-р",
    "8-р", "9-р", "10-р", "11-р", "12-р"};
static char *day_names_mn_MN[] = {
    "Даваа", "Мягмар", "Лхагва", "Пүрэв", "Баасан", "Бямба", "Ням"};
static char *ab_day_names_mn_MN[] = {"Да", "Мя", "Лх", "Пү",
                                                      "Ба", "Бя", "Ня"};
/***** LOCALE END mn_MN *****/

/***** LOCALE BEGIN ms_MY: Malay - Malaysia *****/
static char *month_names_ms_MY[] = {
    "Januari", "Februari",  "Mac",     "April",    "Mei",      "Jun", "Julai",
    "Ogos",    "September", "Oktober", "November", "Disember"};
static char *ab_month_names_ms_MY[] = {
    "Jan",  "Feb", "Mac", "Apr", "Mei", "Jun", "Jul",
    "Ogos", "Sep", "Okt", "Nov", "Dis"};
static char *day_names_ms_MY[] = {
    "Isnin", "Selasa", "Rabu", "Khamis", "Jumaat", "Sabtu", "Ahad"};
static char *ab_day_names_ms_MY[] = {
    "Isn", "Sel", "Rab", "Kha", "Jum", "Sab", "Ahd"};
/***** LOCALE END ms_MY *****/

/***** LOCALE BEGIN nb_NO: Norwegian(Bokml) - Norway *****/
static char *month_names_nb_NO[] = {
    "januar", "februar",   "mars",    "april",    "mai",      "juni", "juli",
    "august", "september", "oktober", "november", "desember"};
static char *ab_month_names_nb_NO[] = {
    "jan", "feb", "mar", "apr", "mai", "jun", "jul",
    "aug", "sep", "okt", "nov", "des"};
static char *day_names_nb_NO[] = {
    "mandag", "tirsdag", "onsdag", "torsdag",
    "fredag", "lørdag",  "søndag"};
static char *ab_day_names_nb_NO[] = {
    "man", "tir", "ons", "tor", "fre", "lør", "søn"};
/***** LOCALE END nb_NO *****/

/***** LOCALE BEGIN nl_NL: Dutch - The Netherlands *****/
static char *month_names_nl_NL[] = {
    "januari",  "februari",  "maart",   "april",    "mei",      "juni", "juli",
    "augustus", "september", "oktober", "november", "december"};
static char *ab_month_names_nl_NL[] = {
    "jan", "feb", "mrt", "apr", "mei", "jun", "jul",
    "aug", "sep", "okt", "nov", "dec"};
static char *day_names_nl_NL[] = {
    "maandag", "dinsdag",  "woensdag", "donderdag",
    "vrijdag", "zaterdag", "zondag"};
static char *ab_day_names_nl_NL[] = {"ma", "di", "wo", "do",
                                                      "vr", "za", "zo"};
/***** LOCALE END nl_NL *****/

/***** LOCALE BEGIN pl_PL: Polish - Poland *****/
static char *month_names_pl_PL[] = {
    "styczeń",  "luty",     "marzec",   "kwiecień", "maj",
    "czerwiec", "lipiec",   "sierpień", "wrzesień", "październik",
    "listopad", "grudzień"};
static char *ab_month_names_pl_PL[] = {
    "sty", "lut", "mar", "kwi", "maj", "cze", "lip",
    "sie", "wrz", "paź", "lis", "gru"};
static char *day_names_pl_PL[] = {
    "poniedziałek", "wtorek", "środa",     "czwartek",
    "piątek",       "sobota", "niedziela"};
static char *ab_day_names_pl_PL[] = {
    "pon", "wto", "śro", "czw", "pią", "sob", "nie"};
/***** LOCALE END pl_PL *****/

/***** LOCALE BEGIN pt_BR: Portugese - Brazil *****/
static char *month_names_pt_BR[] = {
    "janeiro", "fevereiro", "março",   "abril",    "maio",     "junho", "julho",
    "agosto",  "setembro",  "outubro", "novembro", "dezembro"};
static char *ab_month_names_pt_BR[] = {
    "Jan", "Fev", "Mar", "Abr", "Mai", "Jun", "Jul",
    "Ago", "Set", "Out", "Nov", "Dez"};
static char *day_names_pt_BR[] = {"segunda", "terça", "quarta",
                                                   "quinta",  "sexta", "sábado",
                                                   "domingo"};
static char *ab_day_names_pt_BR[] = {
    "Seg", "Ter", "Qua", "Qui", "Sex", "Sáb", "Dom"};
/***** LOCALE END pt_BR *****/

/***** LOCALE BEGIN pt_PT: Portugese - Portugal *****/
static char *month_names_pt_PT[] = {
    "Janeiro", "Fevereiro", "Março",   "Abril",    "Maio",     "Junho", "Julho",
    "Agosto",  "Setembro",  "Outubro", "Novembro", "Dezembro"};
static char *ab_month_names_pt_PT[] = {
    "Jan", "Fev", "Mar", "Abr", "Mai", "Jun", "Jul",
    "Ago", "Set", "Out", "Nov", "Dez"};
static char *day_names_pt_PT[] = {"Segunda", "Terça", "Quarta",
                                                   "Quinta",  "Sexta", "Sábado",
                                                   "Domingo"};
static char *ab_day_names_pt_PT[] = {
    "Seg", "Ter", "Qua", "Qui", "Sex", "Sáb", "Dom"};
/***** LOCALE END pt_PT *****/

/***** LOCALE BEGIN ro_RO: Romanian - Romania *****/
static char *month_names_ro_RO[] = {
    "Ianuarie",  "Februarie", "Martie", "Aprilie",    "Mai",
    "Iunie",     "Iulie",     "August", "Septembrie", "Octombrie",
    "Noiembrie", "Decembrie"};
static char *ab_month_names_ro_RO[] = {
    "ian", "feb", "mar", "apr", "mai", "iun", "iul",
    "aug", "sep", "oct", "nov", "dec"};
static char *day_names_ro_RO[] = {
    "Luni", "Marţi", "Miercuri", "Joi", "Vineri", "Sâmbătă", "Duminică"};
static char *ab_day_names_ro_RO[] = {"Lu", "Ma", "Mi", "Jo",
                                                      "Vi", "Sâ", "Du"};
/***** LOCALE END ro_RO *****/

/***** LOCALE BEGIN ru_RU: Russian - Russia *****/
static char *month_names_ru_RU[] = {
    "Января",  "Февраля",  "Марта",   "Апреля", "Мая",     "Июня", "Июля",
    "Августа", "Сентября", "Октября", "Ноября", "Декабря"};
static char *ab_month_names_ru_RU[] = {
    "Янв", "Фев", "Мар", "Апр", "Май", "Июн", "Июл",
    "Авг", "Сен", "Окт", "Ноя", "Дек"};
static char *day_names_ru_RU[] = {
    "Понедельник", "Вторник", "Среда",       "Четверг",
    "Пятница",     "Суббота", "Воскресенье"};
static char *ab_day_names_ru_RU[] = {
    "Пнд", "Втр", "Срд", "Чтв", "Птн", "Сбт", "Вск"};
/***** LOCALE END ru_RU *****/

/***** LOCALE BEGIN ru_UA: Russian - Ukraine *****/
static char *month_names_ru_UA[] = {
    "Январь", "Февраль",  "Март",    "Апрель", "Май",     "Июнь", "Июль",
    "Август", "Сентябрь", "Октябрь", "Ноябрь", "Декабрь"};
static char *ab_month_names_ru_UA[] = {
    "Янв", "Фев", "Мар", "Апр", "Май", "Июн", "Июл",
    "Авг", "Сен", "Окт", "Ноя", "Дек"};
static char *day_names_ru_UA[] = {
    "Понедельник", "Вторник", "Среда",       "Четверг",
    "Пятница",     "Суббота", "Воскресенье"};
static char *ab_day_names_ru_UA[] = {
    "Пнд", "Вто", "Срд", "Чтв", "Птн", "Суб", "Вск"};
/***** LOCALE END ru_UA *****/

/***** LOCALE BEGIN sk_SK: Slovak - Slovakia *****/
static char *month_names_sk_SK[] = {
    "január", "február",   "marec",   "apríl",    "máj",      "jún", "júl",
    "august", "september", "október", "november", "december"};
static char *ab_month_names_sk_SK[] = {
    "jan", "feb", "mar", "apr", "máj", "jún", "júl",
    "aug", "sep", "okt", "nov", "dec"};
static char *day_names_sk_SK[] = {
    "Pondelok", "Utorok", "Streda", "Štvrtok",
    "Piatok",   "Sobota", "Nedeľa"};
static char *ab_day_names_sk_SK[] = {"Po", "Ut", "St", "Št",
                                                      "Pi", "So", "Ne"};
/***** LOCALE END sk_SK *****/

/***** LOCALE BEGIN sl_SI: Slovenian - Slovenia *****/
static char *month_names_sl_SI[] = {
    "januar", "februar",   "marec",   "april",    "maj",      "junij", "julij",
    "avgust", "september", "oktober", "november", "december"};
static char *ab_month_names_sl_SI[] = {
    "jan", "feb", "mar", "apr", "maj", "jun", "jul",
    "avg", "sep", "okt", "nov", "dec"};
static char *day_names_sl_SI[] = {
    "ponedeljek", "torek",  "sreda",   "četrtek",
    "petek",      "sobota", "nedelja"};
static char *ab_day_names_sl_SI[] = {
    "pon", "tor", "sre", "čet", "pet", "sob", "ned"};
/***** LOCALE END sl_SI *****/

/***** LOCALE BEGIN sq_AL: Albanian - Albania *****/
static char *month_names_sq_AL[] = {
    "janar", "shkurt",  "mars",  "prill",  "maj",     "qershor", "korrik",
    "gusht", "shtator", "tetor", "nëntor", "dhjetor"};
static char *ab_month_names_sq_AL[] = {
    "Jan", "Shk", "Mar", "Pri", "Maj", "Qer", "Kor",
    "Gsh", "Sht", "Tet", "Nën", "Dhj"};
static char *day_names_sq_AL[] = {
    "e hënë ",   "e martë ",  "e mërkurë ", "e enjte ",
    "e premte ", "e shtunë ", "e diel "};
static char *ab_day_names_sq_AL[] = {
    "Hën ", "Mar ", "Mër ", "Enj ", "Pre ", "Sht ", "Die "};
/***** LOCALE END sq_AL *****/

/***** LOCALE BEGIN sr_RS: Serbian - Serbia *****/
static char *month_names_sr_RS[] = {
    "januar", "februar",   "mart",    "april",    "maj",      "juni", "juli",
    "avgust", "septembar", "oktobar", "novembar", "decembar"};
static char *ab_month_names_sr_RS[] = {
    "jan", "feb", "mar", "apr", "maj", "jun", "jul",
    "avg", "sep", "okt", "nov", "dec"};
static char *day_names_sr_RS[] = {
    "ponedeljak", "utorak", "sreda",   "četvrtak",
    "petak",      "subota", "nedelja"};
static char *ab_day_names_sr_RS[] = {
    "pon", "uto", "sre", "čet", "pet", "sub", "ned"};
/***** LOCALE END sr_RS *****/

/***** LOCALE BEGIN sv_SE: Swedish - Sweden *****/
static char *month_names_sv_SE[] = {
    "januari", "februari",  "mars",    "april",    "maj",      "juni", "juli",
    "augusti", "september", "oktober", "november", "december"};
static char *ab_month_names_sv_SE[] = {
    "jan", "feb", "mar", "apr", "maj", "jun", "jul",
    "aug", "sep", "okt", "nov", "dec"};
static char *day_names_sv_SE[] = {
    "måndag", "tisdag", "onsdag", "torsdag",
    "fredag", "lördag", "söndag"};
static char *ab_day_names_sv_SE[] = {
    "mån", "tis", "ons", "tor", "fre", "lör", "sön"};
/***** LOCALE END sv_SE *****/

/***** LOCALE BEGIN ta_IN: Tamil - India *****/
static char *month_names_ta_IN[] = {
    "ஜனவரி", "பெப்ரவரி", "மார்ச்",   "ஏப்ரல்",  "மே",      "ஜூன்", "ஜூலை",
    "ஆகஸ்ட்",  "செப்டம்பர்", "அக்டோபர்", "நவம்பர்", "டிசம்பர்r"};
static char *ab_month_names_ta_IN[] = {
    "ஜனவரி", "பெப்ரவரி", "மார்ச்",   "ஏப்ரல்",  "மே",      "ஜூன்", "ஜூலை",
    "ஆகஸ்ட்",  "செப்டம்பர்", "அக்டோபர்", "நவம்பர்", "டிசம்பர்r"};
static char *day_names_ta_IN[] = {
    "திங்கள்", "செவ்வாய்", "புதன்", "வியாழன்", "வெள்ளி", "சனி", "ஞாயிறு"};
static char *ab_day_names_ta_IN[] = {"த", "ச", "ப", "வ",
                                                      "வ", "ச", "ஞ"};
/***** LOCALE END ta_IN *****/

/***** LOCALE BEGIN te_IN: Telugu - India *****/
static char *month_names_te_IN[] = {
    "జనవరి",  "ఫిబ్రవరి",  "మార్చి",   "ఏప్రిల్",  "మే",     "జూన్", "జూలై",
    "ఆగస్టు", "సెప్టెంబర్", "అక్టోబర్", "నవంబర్", "డిసెంబర్"};
static char *ab_month_names_te_IN[] = {
    "జనవరి",  "ఫిబ్రవరి",  "మార్చి",   "ఏప్రిల్",  "మే",     "జూన్", "జూలై",
    "ఆగస్టు", "సెప్టెంబర్", "అక్టోబర్", "నవంబర్", "డిసెంబర్"};
static char *day_names_te_IN[] = {
    "సోమవారం",   "మంగళవారం", "బుధవారం", "గురువారం",
    "శుక్రవారం", "శనివారం",   "ఆదివారం"};
static char *ab_day_names_te_IN[] = {
    "సోమ", "మంగళ", "బుధ", "గురు", "శుక్ర", "శని", "ఆది"};
/***** LOCALE END te_IN *****/

/***** LOCALE BEGIN th_TH: Thai - Thailand *****/
static char *month_names_th_TH[] = {
    "มกราคม", "กุมภาพันธ์", "มีนาคม", "เมษายน",   "พฤษภาคม", "มิถุนายน", "กรกฎาคม",
    "สิงหาคม", "กันยายน",  "ตุลาคม", "พฤศจิกายน", "ธันวาคม"};
static char *ab_month_names_th_TH[] = {
    "ม.ค.", "ก.พ.", "มี.ค.", "เม.ย.", "พ.ค.", "มิ.ย.", "ก.ค.",
    "ส.ค.", "ก.ย.", "ต.ค.", "พ.ย.",  "ธ.ค."};
static char *day_names_th_TH[] = {
    "จันทร์", "อังคาร", "พุธ", "พฤหัสบดี", "ศุกร์", "เสาร์", "อาทิตย์"};
static char *ab_day_names_th_TH[] = {"จ.", "อ.", "พ.",  "พฤ.",
                                                      "ศ.", "ส.", "อา."};
/***** LOCALE END th_TH *****/

/***** LOCALE BEGIN tr_TR: Turkish - Turkey *****/
static char *month_names_tr_TR[] = {
    "Ocak",    "Şubat", "Mart", "Nisan", "Mayıs",  "Haziran", "Temmuz",
    "Ağustos", "Eylül", "Ekim", "Kasım", "Aralık"};
static char *ab_month_names_tr_TR[] = {
    "Oca", "Şub", "Mar", "Nis", "May", "Haz", "Tem",
    "Ağu", "Eyl", "Eki", "Kas", "Ara"};
static char *day_names_tr_TR[] = {
    "Pazartesi", "Salı",      "Çarşamba", "Perşembe",
    "Cuma",      "Cumartesi", "Pazar"};
static char *ab_day_names_tr_TR[] = {
    "Pzt", "Sal", "Çrş", "Prş", "Cum", "Cts", "Paz"};
/***** LOCALE END tr_TR *****/

/***** LOCALE BEGIN uk_UA: Ukrainian - Ukraine *****/
static char *month_names_uk_UA[] = {
    "Січень",   "Лютий",   "Березень", "Квітень",  "Травень",
    "Червень",  "Липень",  "Серпень",  "Вересень", "Жовтень",
    "Листопад", "Грудень"};
static char *ab_month_names_uk_UA[] = {
    "Січ", "Лют", "Бер", "Кві", "Тра", "Чер", "Лип",
    "Сер", "Вер", "Жов", "Лис", "Гру"};
static char *day_names_uk_UA[] = {
    "Понеділок", "Вівторок", "Середа", "Четвер",
    "П'ятниця",  "Субота",   "Неділя"};
static char *ab_day_names_uk_UA[] = {
    "Пнд", "Втр", "Срд", "Чтв", "Птн", "Сбт", "Ндл"};
/***** LOCALE END uk_UA *****/

/***** LOCALE BEGIN ur_PK: Urdu - Pakistan *****/
static char *month_names_ur_PK[] = {
    "جنوري", "فروري", "مارچ",   "اپريل", "مٓی",    "جون", "جولاي",
    "اگست",  "ستمبر", "اكتوبر", "نومبر", "دسمبر"};
static char *ab_month_names_ur_PK[] = {
    "جنوري", "فروري", "مارچ",   "اپريل", "مٓی",    "جون", "جولاي",
    "اگست",  "ستمبر", "اكتوبر", "نومبر", "دسمبر"};
static char *day_names_ur_PK[] = {
    "پير", "منگل", "بدھ", "جمعرات", "جمعه", "هفته", "اتوار"};
static char *ab_day_names_ur_PK[] = {
    "پير", "منگل", "بدھ", "جمعرات", "جمعه", "هفته", "اتوار"};
/***** LOCALE END ur_PK *****/

/***** LOCALE BEGIN vi_VN: Vietnamese - Vietnam *****/
static char *month_names_vi_VN[] = {
    "Tháng một",      "Tháng hai",      "Tháng ba",  "Tháng tư",   "Tháng năm",
    "Tháng sáu",      "Tháng bảy",      "Tháng tám", "Tháng chín", "Tháng mười",
    "Tháng mười một", "Tháng mười hai"};
static char *ab_month_names_vi_VN[] = {
    "Thg 1", "Thg 2", "Thg 3",  "Thg 4",  "Thg 5",  "Thg 6", "Thg 7",
    "Thg 8", "Thg 9", "Thg 10", "Thg 11", "Thg 12"};
static char *day_names_vi_VN[] = {
    "Thứ hai ", "Thứ ba ",  "Thứ tư ",   "Thứ năm ",
    "Thứ sáu ", "Thứ bảy ", "Chủ nhật "};
static char *ab_day_names_vi_VN[] = {
    "Th 2 ", "Th 3 ", "Th 4 ", "Th 5 ", "Th 6 ", "Th 7 ", "CN "};
/***** LOCALE END vi_VN *****/

/***** LOCALE BEGIN zh_CN: Chinese - Peoples Republic of China *****/
static char *month_names_zh_CN[] = {
    "一月", "二月", "三月", "四月",   "五月",   "六月", "七月",
    "八月", "九月", "十月", "十一月", "十二月"};
static char *ab_month_names_zh_CN[] = {
    " 1月", " 2月", " 3月", " 4月", " 5月", " 6月", " 7月",
    " 8月", " 9月", "10月", "11月", "12月"};
static char *day_names_zh_CN[] = {"星期一", "星期二", "星期三",
                                                   "星期四", "星期五", "星期六",
                                                   "星期日"};
static char *ab_day_names_zh_CN[] = {"一", "二", "三", "四",
                                                      "五", "六", "日"};
/***** LOCALE END zh_CN *****/

/***** LOCALE BEGIN zh_TW: Chinese - Taiwan *****/
static char *month_names_zh_TW[] = {
    "一月", "二月", "三月", "四月",   "五月",   "六月", "七月",
    "八月", "九月", "十月", "十一月", "十二月"};
static char *ab_month_names_zh_TW[] = {
    " 1月", " 2月", " 3月", " 4月", " 5月", " 6月", " 7月",
    " 8月", " 9月", "10月", "11月", "12月"};
static char *day_names_zh_TW[] = {
    "週一", "週二", "週三", "週四", "週五", "週六", "週日"};
static char *ab_day_names_zh_TW[] = {"一", "二", "三", "四",
                                                      "五", "六", "日"};
/***** LOCALE END zh_TW *****/

/***** LOCALE BEGIN el_GR: Greek - Greece *****/
static char *month_names_el_GR[] = {
    "Ιανουάριος", "Φεβρουάριος", "Μάρτιος",   "Απρίλιος",    "Μάιος",
    "Ιούνιος",    "Ιούλιος",     "Αύγουστος", "Σεπτέμβριος", "Οκτώβριος",
    "Νοέμβριος",  "Δεκέμβριος"};

static char *ab_month_names_el_GR[] = {
    "Ιαν", "Φεβ", "Μάρ", "Απρ", "Μάι", "Ιούν", "Ιούλ",
    "Αύγ", "Σεπ", "Οκτ", "Νοέ", "Δεκ"};

static char *day_names_el_GR[] = {
    "Δευτέρα",   "Τρίτη",   "Τετάρτη", "Πέμπτη",
    "Παρασκευή", "Σάββατο", "Κυριακή"};

static char *ab_day_names_el_GR[] = {
    "Δευ", "Τρί", "Τετ", "Πέμ", "Παρ", "Σάβ", "Κυρ"};
/***** LOCALE END el_GR *****/

/***** LOCALE BEGIN rm_CH: Romansh - Switzerland *****/
static char *month_names_rm_CH[] = {
    "schaner",   "favrer",   "mars",  "avrigl",    "matg",
    "zercladur", "fanadur",  "avust", "settember", "october",
    "november",  "december"};

static char *ab_month_names_rm_CH[] = {
    "schan", "favr", "mars", "avr", "matg", "zercl", "fan",
    "avust", "sett", "oct",  "nov", "dec"};

static char *day_names_rm_CH[] = {
    "glindesdi", "mardi", "mesemna",  "gievgia",
    "venderdi",  "sonda", "dumengia"};

static char *ab_day_names_rm_CH[] = {"gli", "ma", "me", "gie",
                                                      "ve",  "so", "du"};
/***** LOCALE END rm_CH *****/

#endif

/* please ensure that locale_name is in order */
static MyLocale my_locale_array[] = {
#ifdef DOLPHIN
    {"ar_AE", '.', ',', month_names_ar_AE, ab_month_names_ar_AE, day_names_ar_AE, ab_day_names_ar_AE},
    {"ar_BH", '.', ',', month_names_ar_BH, ab_month_names_ar_BH, day_names_ar_BH, ab_day_names_ar_BH},
    {"ar_DZ", '.', ',', month_names_ar_BH, ab_month_names_ar_BH, day_names_ar_BH, ab_day_names_ar_BH},
    {"ar_EG", '.', ',', month_names_ar_BH, ab_month_names_ar_BH, day_names_ar_BH, ab_day_names_ar_BH},
    {"ar_IN", '.', ',', month_names_ar_BH, ab_month_names_ar_BH, day_names_ar_BH, ab_day_names_ar_BH},
    {"ar_IQ", '.', ',', month_names_ar_BH, ab_month_names_ar_BH, day_names_ar_BH, ab_day_names_ar_BH},
    {"ar_JO", '.', ',', month_names_ar_JO, ab_month_names_ar_JO, day_names_ar_JO, ab_day_names_ar_JO},
    {"ar_KW", '.', ',', month_names_ar_BH, ab_month_names_ar_BH, day_names_ar_BH, ab_day_names_ar_BH},
    {"ar_LB", '.', ',', month_names_ar_JO, ab_month_names_ar_JO, day_names_ar_JO, ab_day_names_ar_JO},
    {"ar_LY", '.', ',', month_names_ar_BH, ab_month_names_ar_BH, day_names_ar_BH, ab_day_names_ar_BH},
    {"ar_MA", '.', ',', month_names_ar_BH, ab_month_names_ar_BH, day_names_ar_BH, ab_day_names_ar_BH},
    {"ar_OM", '.', ',', month_names_ar_BH, ab_month_names_ar_BH, day_names_ar_BH, ab_day_names_ar_BH},
    {"ar_QA", '.', ',', month_names_ar_BH, ab_month_names_ar_BH, day_names_ar_BH, ab_day_names_ar_BH},
    {"ar_SA", '.', '\0', month_names_ar_SA, ab_month_names_ar_SA, day_names_ar_SA, ab_day_names_ar_SA},
    {"ar_SD", '.', ',', month_names_ar_BH, ab_month_names_ar_BH, day_names_ar_BH, ab_day_names_ar_BH},
    {"ar_SY", '.', ',', month_names_ar_SY, ab_month_names_ar_SY, day_names_ar_SY, ab_day_names_ar_SY},
    {"ar_TN", '.', ',', month_names_ar_BH, ab_month_names_ar_BH, day_names_ar_BH, ab_day_names_ar_BH},
    {"ar_YE", '.', ',', month_names_ar_BH, ab_month_names_ar_BH, day_names_ar_BH, ab_day_names_ar_BH},
    {"be_BY", ',', '.', month_names_be_BY, ab_month_names_be_BY, day_names_be_BY, ab_day_names_be_BY},
    {"bg_BG", ',', '\0', month_names_bg_BG, ab_month_names_bg_BG, day_names_bg_BG, ab_day_names_bg_BG},
    {"ca_ES", ',', '\0', month_names_ca_ES, ab_month_names_ca_ES, day_names_ca_ES, ab_day_names_ca_ES},
    {"cs_CZ", ',', ' ', month_names_cs_CZ, ab_month_names_cs_CZ, day_names_cs_CZ, ab_day_names_cs_CZ},
    {"da_DK", ',', '.', month_names_da_DK, ab_month_names_da_DK, day_names_da_DK, ab_day_names_da_DK},
    {"de_AT", ',', '\0', month_names_de_AT, ab_month_names_de_AT, day_names_de_AT, ab_day_names_de_AT},
    {"de_BE", ',', '.', month_names_de_DE, ab_month_names_de_DE, day_names_de_DE, ab_day_names_de_DE},
    {"de_CH", '.', '\'', month_names_de_DE, ab_month_names_de_DE, day_names_de_DE, ab_day_names_de_DE},
    {"de_DE", ',', '.', month_names_de_DE, ab_month_names_de_DE, day_names_de_DE, ab_day_names_de_DE},
    {"de_LU", ',', '.', month_names_de_DE, ab_month_names_de_DE, day_names_de_DE, ab_day_names_de_DE},
    {"en_AU", '.', ',', month_names_en_US, ab_month_names_en_US, day_names_en_US, ab_day_names_en_US},
    {"en_CA", '.', ',', month_names_en_US, ab_month_names_en_US, day_names_en_US, ab_day_names_en_US},
    {"en_GB", '.', ',', month_names_en_US, ab_month_names_en_US, day_names_en_US, ab_day_names_en_US},
    {"en_IN", '.', ',', month_names_en_US, ab_month_names_en_US, day_names_en_US, ab_day_names_en_US},
    {"en_NZ", '.', ',', month_names_en_US, ab_month_names_en_US, day_names_en_US, ab_day_names_en_US},
    {"en_PH", '.', ',', month_names_en_US, ab_month_names_en_US, day_names_en_US, ab_day_names_en_US},
    {"en_US", '.', ',', month_names_en_US, ab_month_names_en_US, day_names_en_US, ab_day_names_en_US},
    {"en_ZA", '.', ',', month_names_en_US, ab_month_names_en_US, day_names_en_US, ab_day_names_en_US},
    {"en_ZW", '.', ',', month_names_en_US, ab_month_names_en_US, day_names_en_US, ab_day_names_en_US},
    {"es_AR", ',', '.', month_names_es_ES, ab_month_names_es_ES, day_names_es_ES, ab_day_names_es_ES},
    {"es_BO", ',', '\0', month_names_es_ES, ab_month_names_es_ES, day_names_es_ES, ab_day_names_es_ES},
    {"es_CL", ',', '\0', month_names_es_ES, ab_month_names_es_ES, day_names_es_ES, ab_day_names_es_ES},
    {"es_CO", ',', '\0', month_names_es_ES, ab_month_names_es_ES, day_names_es_ES, ab_day_names_es_ES},
    {"es_CR", '.', '\0', month_names_es_ES, ab_month_names_es_ES, day_names_es_ES, ab_day_names_es_ES},
    {"es_DO", '.', '\0', month_names_es_ES, ab_month_names_es_ES, day_names_es_ES, ab_day_names_es_ES},
    {"es_EC", ',', '\0', month_names_es_ES, ab_month_names_es_ES, day_names_es_ES, ab_day_names_es_ES},
    {"es_ES", ',', '\0', month_names_es_ES, ab_month_names_es_ES, day_names_es_ES, ab_day_names_es_ES},
    {"es_GT", '.', '\0', month_names_es_ES, ab_month_names_es_ES, day_names_es_ES, ab_day_names_es_ES},
    {"es_HN", '.', '\0', month_names_es_ES, ab_month_names_es_ES, day_names_es_ES, ab_day_names_es_ES},
    {"es_MX", '.', '\0', month_names_es_ES, ab_month_names_es_ES, day_names_es_ES, ab_day_names_es_ES},
    {"es_NI", '.', '\0', month_names_es_ES, ab_month_names_es_ES, day_names_es_ES, ab_day_names_es_ES},
    {"es_PA", '.', '\0', month_names_es_ES, ab_month_names_es_ES, day_names_es_ES, ab_day_names_es_ES},
    {"es_PE", '.', '\0', month_names_es_ES, ab_month_names_es_ES, day_names_es_ES, ab_day_names_es_ES},
    {"es_PR", '.', '\0', month_names_es_ES, ab_month_names_es_ES, day_names_es_ES, ab_day_names_es_ES},
    {"es_PY", ',', '\0', month_names_es_ES, ab_month_names_es_ES, day_names_es_ES, ab_day_names_es_ES},
    {"es_SV", '.', '\0', month_names_es_ES, ab_month_names_es_ES, day_names_es_ES, ab_day_names_es_ES},
    {"es_US", '.', ',', month_names_es_ES, ab_month_names_es_ES, day_names_es_ES, ab_day_names_es_ES},
    {"es_UY", ',', '\0', month_names_es_ES, ab_month_names_es_ES, day_names_es_ES, ab_day_names_es_ES},
    {"es_VE", ',', '\0', month_names_es_ES, ab_month_names_es_ES, day_names_es_ES, ab_day_names_es_ES},
    {"el_GR", ',', '.', month_names_el_GR, ab_month_names_el_GR, day_names_el_GR, ab_day_names_el_GR},
    {"et_EE", ',', ' ', month_names_et_EE, ab_month_names_et_EE, day_names_et_EE, ab_day_names_et_EE},
    {"eu_ES", ',', '\0', month_names_eu_ES, ab_month_names_eu_ES, day_names_eu_ES, ab_day_names_eu_ES},
    {"fi_FI", ',', ' ', month_names_fi_FI, ab_month_names_fi_FI, day_names_fi_FI, ab_day_names_fi_FI},
    {"fo_FO", ',', '.', month_names_fo_FO, ab_month_names_fo_FO, day_names_fo_FO, ab_day_names_fo_FO},
    {"fr_BE", ',', '.', month_names_fr_FR, ab_month_names_fr_FR, day_names_fr_FR, ab_day_names_fr_FR},
    {"fr_CA", ',', ' ', month_names_fr_FR, ab_month_names_fr_FR, day_names_fr_FR, ab_day_names_fr_FR},
    {"fr_CH", ',', '\0', month_names_fr_FR, ab_month_names_fr_FR, day_names_fr_FR, ab_day_names_fr_FR},
    {"fr_FR", ',', '\0', month_names_fr_FR, ab_month_names_fr_FR, day_names_fr_FR, ab_day_names_fr_FR},
    {"fr_LU", ',', '\0', month_names_fr_FR, ab_month_names_fr_FR, day_names_fr_FR, ab_day_names_fr_FR},
    {"gl_ES", ',', '\0', month_names_gl_ES, ab_month_names_gl_ES, day_names_gl_ES, ab_day_names_gl_ES},
    {"gu_IN", '.', ',', month_names_gu_IN, ab_month_names_gu_IN, day_names_gu_IN, ab_day_names_gu_IN},
    {"he_IL", '.', ',', month_names_he_IL, ab_month_names_he_IL, day_names_he_IL, ab_day_names_he_IL},
    {"hi_IN", '.', ',', month_names_hi_IN, ab_month_names_hi_IN, day_names_hi_IN, ab_day_names_hi_IN},
    {"hr_HR", ',', '\0', month_names_hr_HR, ab_month_names_hr_HR, day_names_hr_HR, ab_day_names_hr_HR},
    {"hu_HU", ',', '.', month_names_hu_HU, ab_month_names_hu_HU, day_names_hu_HU, ab_day_names_hu_HU},
    {"id_ID", ',', '.', month_names_id_ID, ab_month_names_id_ID, day_names_id_ID, ab_day_names_id_ID},
    {"is_IS", ',', '.', month_names_is_IS, ab_month_names_is_IS, day_names_is_IS, ab_day_names_is_IS},
    {"it_CH", ',', '\'', month_names_it_CH, ab_month_names_it_CH, day_names_it_CH, ab_day_names_it_CH},
    {"it_IT", ',', '\0', month_names_it_CH, ab_month_names_it_CH, day_names_it_CH, ab_day_names_it_CH},
    {"ja_JP", '.', ',', month_names_ja_JP, ab_month_names_ja_JP, day_names_ja_JP, ab_day_names_ja_JP},
    {"ko_KR", '.', ',', month_names_ko_KR, ab_month_names_ko_KR, day_names_ko_KR, ab_day_names_ko_KR},
    {"lt_LT", ',', '.', month_names_lt_LT, ab_month_names_lt_LT, day_names_lt_LT, ab_day_names_lt_LT},
    {"lv_LV", ',', ' ', month_names_lv_LV, ab_month_names_lv_LV, day_names_lv_LV, ab_day_names_lv_LV},
    {"mk_MK", ',', ' ', month_names_mk_MK, ab_month_names_mk_MK, day_names_mk_MK, ab_day_names_mk_MK},
    {"mn_MN", ',', '.', month_names_mn_MN, ab_month_names_mn_MN, day_names_mn_MN, ab_day_names_mn_MN},
    {"ms_MY", '.', ',', month_names_ms_MY, ab_month_names_ms_MY, day_names_ms_MY, ab_day_names_ms_MY},
    {"nb_NO", ',', '.', month_names_nb_NO, ab_month_names_nb_NO, day_names_nb_NO, ab_day_names_nb_NO},
    {"nl_BE", ',', '.', month_names_nl_NL, ab_month_names_nl_NL, day_names_nl_NL, ab_day_names_nl_NL},
    {"nl_NL", ',', '\0', month_names_nl_NL, ab_month_names_nl_NL, day_names_nl_NL, ab_day_names_nl_NL},
    {"no_NO", ',', '.', month_names_nb_NO, ab_month_names_nb_NO, day_names_nb_NO, ab_day_names_nb_NO},
    {"pl_PL", ',', '\0', month_names_pl_PL, ab_month_names_pl_PL, day_names_pl_PL, ab_day_names_pl_PL},
    {"pt_BR", ',', '\0', month_names_pt_BR, ab_month_names_pt_BR, day_names_pt_BR, ab_day_names_pt_BR},
    {"pt_PT", ',', '\0', month_names_pt_PT, ab_month_names_pt_PT, day_names_pt_PT, ab_day_names_pt_PT},
    {"rm_CH", ',', '\'', month_names_rm_CH, ab_month_names_rm_CH, day_names_rm_CH, ab_day_names_rm_CH},
    {"ro_RO", ',', '.', month_names_ro_RO, ab_month_names_ro_RO, day_names_ro_RO, ab_day_names_ro_RO},
    {"ru_RU", ',', ' ', month_names_ru_RU, ab_month_names_ru_RU, day_names_ru_RU, ab_day_names_ru_RU},
    {"ru_UA", ',', '.', month_names_ru_UA, ab_month_names_ru_UA, day_names_ru_UA, ab_day_names_ru_UA},
    {"sk_SK", ',', ' ', month_names_sk_SK, ab_month_names_sk_SK, day_names_sk_SK, ab_day_names_sk_SK},
    {"sl_SI", ',', ' ', month_names_sl_SI, ab_month_names_sl_SI, day_names_sl_SI, ab_day_names_sl_SI},
    {"sq_AL", ',', '.', month_names_sq_AL, ab_month_names_sq_AL, day_names_sq_AL, ab_day_names_sq_AL},
    {"sr_RS", '.', '\0', month_names_sr_RS, ab_month_names_sr_RS, day_names_sr_RS, ab_day_names_sr_RS},
    {"sv_FI", ',', ' ', month_names_sv_SE, ab_month_names_sv_SE, day_names_sv_SE, ab_day_names_sv_SE},
    {"sv_SE", ',', ' ', month_names_sv_SE, ab_month_names_sv_SE, day_names_sv_SE, ab_day_names_sv_SE},
    {"ta_IN", '.', ',', month_names_ta_IN, ab_month_names_ta_IN, day_names_ta_IN, ab_day_names_ta_IN},
    {"te_IN", '.', ',', month_names_te_IN, ab_month_names_te_IN, day_names_te_IN, ab_day_names_te_IN},
    {"th_TH", '.', ',', month_names_th_TH, ab_month_names_th_TH, day_names_th_TH, ab_day_names_th_TH},
    {"tr_TR", ',', '.', month_names_tr_TR, ab_month_names_tr_TR, day_names_tr_TR, ab_day_names_tr_TR},
    {"uk_UA", ',', '.', month_names_uk_UA, ab_month_names_uk_UA, day_names_uk_UA, ab_day_names_uk_UA},
    {"ur_PK", '.', ',', month_names_ur_PK, ab_month_names_ur_PK, day_names_ur_PK, ab_day_names_ur_PK},
    {"vi_VN", ',', '.', month_names_vi_VN, ab_month_names_vi_VN, day_names_vi_VN, ab_day_names_vi_VN},
    {"zh_CN", '.', ',', month_names_zh_CN, ab_month_names_zh_CN, day_names_zh_CN, ab_day_names_zh_CN},
    {"zh_HK", '.', ',', month_names_zh_CN, ab_month_names_zh_CN, day_names_zh_CN, ab_day_names_zh_CN},
    {"zh_TW", '.', ',', month_names_zh_TW, ab_month_names_zh_TW, day_names_zh_TW, ab_day_names_zh_TW},
#else
    {"ar_AE", '.', ','},
    {"ar_BH", '.', ','},
    {"ar_DZ", '.', ','},
    {"ar_EG", '.', ','},
    {"ar_IN", '.', ','},
    {"ar_IQ", '.', ','},
    {"ar_JO", '.', ','},
    {"ar_KW", '.', ','},
    {"ar_LB", '.', ','},
    {"ar_LY", '.', ','},
    {"ar_MA", '.', ','},
    {"ar_OM", '.', ','},
    {"ar_QA", '.', ','},
    {"ar_SA", '.', '\0'},
    {"ar_SD", '.', ','},
    {"ar_SY", '.', ','},
    {"ar_TN", '.', ','},
    {"ar_YE", '.', ','},
    {"be_BY", ',', '.'},
    {"bg_BG", ',', '\0'},
    {"ca_ES", ',', '\0'},
    {"cs_CZ", ',', ' '},
    {"da_DK", ',', '.'},
    {"de_AT", ',', '\0'},
    {"de_BE", ',', '.'},
    {"de_CH", '.', '\''},
    {"de_DE", ',', '.'},
    {"de_LU", ',', '.'},
    {"en_AU", '.', ','},
    {"en_CA", '.', ','},
    {"en_GB", '.', ','},
    {"en_IN", '.', ','},
    {"en_NZ", '.', ','},
    {"en_PH", '.', ','},
    {"en_US", '.', ','},
    {"en_ZA", '.', ','},
    {"en_ZW", '.', ','},
    {"es_AR", ',', '.'},
    {"es_BO", ',', '\0'},
    {"es_CL", ',', '\0'},
    {"es_CO", ',', '\0'},
    {"es_CR", '.', '\0'},
    {"es_DO", '.', '\0'},
    {"es_EC", ',', '\0'},
    {"es_ES", ',', '\0'},
    {"es_GT", '.', '\0'},
    {"es_HN", '.', '\0'},
    {"es_MX", '.', '\0'},
    {"es_NI", '.', '\0'},
    {"es_PA", '.', '\0'},
    {"es_PE", '.', '\0'},
    {"es_PR", '.', '\0'},
    {"es_PY", ',', '\0'},
    {"es_SV", '.', '\0'},
    {"es_US", '.', ','},
    {"es_UY", ',', '\0'},
    {"es_VE", ',', '\0'},
    {"el_GR", ',', '.'},
    {"et_EE", ',', ' '},
    {"eu_ES", ',', '\0'},
    {"fi_FI", ',', ' '},
    {"fo_FO", ',', '.'},
    {"fr_BE", ',', '.'},
    {"fr_CA", ',', ' '},
    {"fr_CH", ',', '\0'},
    {"fr_FR", ',', '\0'},
    {"fr_LU", ',', '\0'},
    {"gl_ES", ',', '\0'},
    {"gu_IN", '.', ','},
    {"he_IL", '.', ','},
    {"hi_IN", '.', ','},
    {"hr_HR", ',', '\0'},
    {"hu_HU", ',', '.'},
    {"id_ID", ',', '.'},
    {"is_IS", ',', '.'},
    {"it_CH", ',', '\''},
    {"it_IT", ',', '\0'},
    {"ja_JP", '.', ','},
    {"ko_KR", '.', ','},
    {"lt_LT", ',', '.'},
    {"lv_LV", ',', ' '},
    {"mk_MK", ',', ' '},
    {"mn_MN", ',', '.'},
    {"ms_MY", '.', ','},
    {"nb_NO", ',', '.'},
    {"nl_BE", ',', '.'},
    {"nl_NL", ',', '\0'},
    {"no_NO", ',', '.'},
    {"pl_PL", ',', '\0'},
    {"pt_BR", ',', '\0'},
    {"pt_PT", ',', '\0'},
    {"rm_CH", ',', '\''},
    {"ro_RO", ',', '.'},
    {"ru_RU", ',', ' '},
    {"ru_UA", ',', '.'},
    {"sk_SK", ',', ' '},
    {"sl_SI", ',', ' '},
    {"sq_AL", ',', '.'},
    {"sr_RS", '.', '\0'},
    {"sv_FI", ',', ' '},
    {"sv_SE", ',', ' '},
    {"ta_IN", '.', ','},
    {"te_IN", '.', ','},
    {"th_TH", '.', ','},
    {"tr_TR", ',', '.'},
    {"uk_UA", ',', '.'},
    {"ur_PK", '.', ','},
    {"vi_VN", ',', '.'},
    {"zh_CN", '.', ','},
    {"zh_HK", '.', ','},
    {"zh_TW", '.', ','},
#endif
};

constexpr int LOCALE_NUM = sizeof(my_locale_array) / sizeof(MyLocale);

MyLocale* MyLocaleSearch(char* target)
{
    int start = 0;
    int end = LOCALE_NUM - 1;

    while (start <= end) {
        int mid = (start + end) / 2;
        int cmp_res = strcmp(target, my_locale_array[mid].locale_name);
        if (cmp_res > 0) {
            start = mid + 1;
        } else if (cmp_res < 0) {
            end = mid - 1;
        } else {
            return &my_locale_array[mid];
        }
    }
    return NULL;
}