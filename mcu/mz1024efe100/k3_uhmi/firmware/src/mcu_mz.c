#include "mcu_mz.h"

unsigned int _cur_font_index = 0;
unsigned char _cur_pen_size = 0;
GFX_XCHAR _tmp_str[64]={0x0};

unsigned char _font14_data[31][7]={
    "E4B880","E585A5","E58886","E5898D","E599A8","E5AE89","E5BD93","E5BE85","E5BF83","E689AB",
    "E68EA5","E695B0","E696B0","E69C80","E69CAC","E69DBE","E78988","E79086","E794B1","E7ABAF",
    "E7AD89","E7AEA1","E7BB88","E88090","E8A385","E8AFB7","E8B7AF","E8BDBB","E9929F","E9A1B5",
    "EFBC9A",
};
unsigned char _font16_data[47][7]={
    "E4B880","E4B889","E4BA8C","E4BA94","E4BBB6","E4BDBF","E585AD","E5898D","E58AA8","E58D87",
    "E58F91","E58F96","E58FB7","E591A8","E59B9B","E59CB0","E59D80","E59E8B","E59F8E","E5A4B1",
    "E5B882","E5BD93","E5BE80","E68896","E6898B","E68BA9","E69690","E696B0","E697A5","E69CAC",
    "E78988","E78EB0","E79086","E794A8","E794B1","E7AEA1","E7BAA7","E88EB7","E8AEAF","E8AFB7",
    "E8B4A5","E8B7AF","E8BDAF","E98089","E99DA2","E9A1B5","EFBC9A",
};
unsigned char _font18_data[20][7]={
    "E4B88A","E4B88B","E4BBB6","E58FB7","E5908D","E59CB0","E59D80","E59E8B","E5AEA2","E5AF86",
    "E697A0","E69CAC","E78988","E7A081","E7A7B0","E7BABF","E8A18C","E8AEBF","E8BDAF","EFBC9A",
};
unsigned char _font18_data_weather[35][7]={
    "E4B8AD","E4BA91","E4BCB4","E586B0","E586BB","E58DB7","E5A49A","E5A4A7","E5A4B9","E5B08F",
    "E5B098","E5B8A6","E5BCBA","E689AC","E699B4","E69AB4","E69C89","E69CAA","E6B299","E6B5AE",
    "E783AD","E789B9","E79FA5","E983A8","E997B4","E998B4","E998B5","E99BA8","E99BAA","E99BB7",
    "E99BBE","E99CBE","E9A38E","E9A393","E9BE99",
};
unsigned char _font20_data[13][7]={
    "E4B88D","E4BFA1","E585B3","E58FB7","E5B7B2","E68EA5","E697A0","E7BABF","E7BB9C","E7BD91",
    "E883BD","E8BF9E","E997AD",
};
unsigned char _font22_data[7][7]={
    "E58F8C","E599A8","E697A0","E794B1","E7BABF","E8B7AF","E9A291",
};
unsigned char _font24_data[28][7]={
    "E4B8AD","E585B3","E587BA","E58D87","E58E82","E590AF","E599A8","E5A48D","E5A4B1","E5B195",
    "E5B7B2","E5BC80","E681A2","E689A9","E697A0","E794B1","E7B3BB","E7BAA7","E7BABF","E7BB9C",
    "E7BB9F","E7BD91","E7BDAE","E8AEBE","E8B4A5","E8B7AF","E9878D","E997AD",
};
unsigned char _font26_data[46][7]={
    "C2B0","E4B880","E4B8AD","E4BBB6","E4BFA1","E585A5","E585B3","E587BA","E58E82","E58F8C",
    "E58FB7","E59088","E590AF","E599A8","E5A48D","E5A4A9","E5A4B1","E5B7B2","E5BC80","E5BC8F",
    "E681A2","E681AF","E68AA5","E68EA5","E696B0","E697A0","E69BB4","E6A8A1","E6B094","E794A8",
    "E794B1","E7ABAF","E7BABF","E7BB88","E7BB9C","E7BBA7","E7BD91","E7BDAE","E8AEBE","E8B4A5",
    "E8B7AF","E8BDAF","E9878D","E997AD","E9A284","E9A291",
};
unsigned char _font18_data_city[1114][7]={
    "E4B881","E4B883","E4B887","E4B888","E4B889","E4B88A","E4B88B","E4B894","E4B898","E4B89A",
    "E4B89C","E4B8A4","E4B8AA","E4B8AD","E4B8B0","E4B8B4","E4B8B9","E4B8BA","E4B8BB","E4B8BD",
    "E4B983","E4B985","E4B989","E4B98C","E4B990","E4B99D","E4B9A0","E4B9A1","E4B9B3","E4B9BE",
    "E4BA8C","E4BA8E","E4BA91","E4BA92","E4BA94","E4BA95","E4BA9A","E4BAA4","E4BAA8","E4BAAC",
    "E4BAAD","E4BAB3","E4BB80","E4BB81","E4BB86","E4BB8B","E4BB8E","E4BB91","E4BB93","E4BB94",
    "E4BB99","E4BBA3","E4BBA4","E4BBAA","E4BBAC","E4BBB2","E4BBBB","E4BC8A","E4BC91","E4BC9A",
    "E4BCA6","E4BCAF","E4BCBD","E4BD98","E4BD99","E4BD9B","E4BD9C","E4BDB3","E4BE9D","E4BEAF",
    "E4BF9D","E4BFA1","E4BFAE","E58183","E5818F","E5848B","E584BF","E58583","E58585","E58589",
    "E5858B","E58596","E5859A","E585A8","E585AB","E585AC","E585AD","E585B0","E585B1","E585B3",
    "E585B4","E585B5","E58680","E58685","E58688","E5868C","E58695","E5869C","E586A0","E586B2",
    "E586B6","E586B7","E58786","E58789","E5878C","E587A4","E587AD","E587AF","E587B0","E58880",
    "E58886","E58899","E5889A","E588A9","E5898D","E58991","E58A9B","E58A9D","E58A9F","E58AA0",
    "E58AA1","E58AA9","E58B83","E58B89","E58B90","E58B92","E58BA4","E58C80","E58C85","E58C96",
    "E58C97","E58CBA","E58D81","E58D83","E58D8E","E58D93","E58D95","E58D97","E58D9A","E58DA1",
    "E58DA2","E58DAB","E58DB0","E58DB3","E58E82","E58E9F","E58EA2","E58EA6","E58EBF","E58F8B",
    "E58F8C","E58F99","E58FA3","E58FA4","E58FA5","E58FAC","E58FB0","E58FB3","E58FB6","E59088",
    "E59089","E5908C","E5908D","E5908E","E59090","E59095","E5909B","E590AB","E590AF","E590B4",
    "E590BE","E59188","E591A8","E591BC","E5928C","E592B8","E59388","E5938D","E59490","E59586",
    "E59680","E59684","E59687","E5969C","E59889","E5988E","E598B4","E599B6","E59B8A","E59B9B",
    "E59B9E","E59BA2","E59BAD","E59BB4","E59BBA","E59BBD","E59BBE","E59C9F","E59CB3","E59CBA",
    "E59D82","E59D8A","E59D8E","E59D9B","E59D9D","E59DA1","E59DA4","E59DAA","E59DBB","E59E92",
    "E59EA3","E59EA6","E59EAB","E59F8E","E59F94","E59FA0","E59FBA","E5A082","E5A086","E5A0A1",
    "E5A0B0","E5A194","E5A198","E5A19E","E5A29E","E5A2A8","E5A381","E5A3A4","E5A3B6","E5A48F",
    "E5A49A","E5A4A7","E5A4A9","E5A4AA","E5A4B4","E5A4B7","E5A4B9","E5A587","E5A588","E5A589",
    "E5A58E","E5A682","E5A683","E5A78B","E5A791","E5A79A","E5A79C","E5A881","E5A884","E5A9BA",
    "E5ABA9","E5AD90","E5AD99","E5AD9A","E5AD9C","E5AD9D","E5AD9F","E5ADAA","E5AE81","E5AE87",
    "E5AE89","E5AE8F","E5AE95","E5AE97","E5AE9A","E5AE9C","E5AE9D","E5AEA1","E5AEA3","E5AEAB",
    "E5AEB6","E5AEB9","E5AEBD","E5AEBE","E5AEBF","E5AF86","E5AF8C","E5AF9F","E5AFA8","E5AFBA",
    "E5AFBB","E5AFBF","E5B081","E5B084","E5B086","E5B089","E5B08F","E5B094","E5B096","E5B09A",
    "E5B0A4","E5B0A7","E5B0BC","E5B0BE","E5B185","E5B18F","E5B1AF","E5B1B1","E5B1BF","E5B290",
    "E5B291","E5B297","E5B29A","E5B29B","E5B2A2","E5B2A9","E5B2AB","E5B2AD","E5B2B1","E5B2B3",
    "E5B2B7","E5B384","E5B392","E5B399","E5B3A1","E5B3A8","E5B3AA","E5B3B0","E5B3BB","E5B482",
    "E5B483","E5B486","E5B487","E5B496","E5B58A","E5B5A9","E5B78D","E5B79D","E5B79E","E5B7A2",
    "E5B7A5","E5B7A6","E5B7A7","E5B7A8","E5B7A9","E5B7AB","E5B7B4","E5B882","E5B883","E5B888",
    "E5B88C","E5B895","E5B8B8","E5B9B2","E5B9B3","E5B9B4","E5B9BF","E5BA84","E5BA86","E5BA90",
    "E5BA93","E5BA94","E5BA95","E5BA97","E5BA9C","E5BAA6","E5BAB7","E5BB89","E5BB8A","E5BBB6",
    "E5BBBA","E5BC80","E5BC8B","E5BC93","E5BCA0","E5BCA5","E5BCBA","E5BD92","E5BD93","E5BD9D",
    "E5BDA6","E5BDAC","E5BDAD","E5BDB0","E5BE81","E5BE90","E5BE92","E5BE97","E5BEAA","E5BEAE",
    "E5BEB7","E5BEBD","E5BF83","E5BF97","E5BFA0","E5BFBB","E68080","E68092","E6809D","E68192",
    "E681A9","E681AD","E681AF","E681B0","E6829F","E683A0","E6849F","E68588","E68888","E68890",
    "E688B4","E688B7","E688BF","E6898E","E68998","E689AC","E689B6","E689BF","E68A95","E68A9A",
    "E68B89","E68B90","E68B96","E68B9B","E68B9C","E68C87","E68E87","E68E96","E68EAA","E68F90",
    "E68FAD","E69480","E694B8","E694B9","E694BF","E69585","E6958F","E69596","E695A6","E69687",
    "E69697","E696AF","E696B0","E696B9","E696BD","E69785","E6978C","E69797","E697A0","E697A5",
    "E697A6","E697A7","E697AC","E697BA","E69882","E69886","E6988C","E6988E","E69893","E69894",
    "E6989F","E698A5","E698AD","E69983","E6998B","E6998F","E699AE","E699AF","E699B4","E69AA8",
    "E69BB2","E69BB9","E69BBC","E69C90","E69C94","E69C97","E69C9B","E69C9D","E69CA8","E69CAB",
    "E69CAC","E69CAD","E69CB1","E69D82","E69D83","E69D91","E69D9C","E69D9E","E69DA5","E69DA8",
    "E69DAD","E69DBE","E69DBF","E69E81","E69E97","E69E9C","E69E9D","E69E9E","E69EA3","E69EB6",
    "E69F8F","E69F94","E69F98","E69F9E","E69FA5","E69FAF","E69FB1","E69FB3","E69FB4","E6A091",
    "E6A096","E6A097","E6A0AA","E6A0B9","E6A0BC","E6A0BE","E6A182","E6A183","E6A190","E6A191",
    "E6A193","E6A1A5","E6A1A6","E6A281","E6A285","E6A293","E6A2A6","E6A2A7","E6A2A8","E6A389",
    "E6A3A3","E6A3B1","E6A48D","E6A492","E6A59A","E6A5BC","E6A686","E6A695","E6A89F","E6A8AA",
    "E6ACA1","E6AD99","E6ADA3","E6ADA5","E6ADA6","E6AF94","E6AF95","E6B08F","E6B091","E6B0B4",
    "E6B0B8","E6B0B9","E6B180","E6B187","E6B189","E6B195","E6B19D","E6B19F","E6B1A0","E6B1A4",
    "E6B1A8","E6B1AA","E6B1B6","E6B1BE","E6B281","E6B282","E6B283","E6B285","E6B288","E6B290",
    "E6B299","E6B29B","E6B29F","E6B2A7","E6B2AD","E6B2B3","E6B2B9","E6B2BB","E6B2BD","E6B2BE",
    "E6B2BF","E6B389","E6B38A","E6B38C","E6B395","E6B397","E6B3A2","E6B3B0","E6B3B8","E6B3BD",
    "E6B3BE","E6B48B","E6B49B","E6B49E","E6B4A5","E6B4AA","E6B4AE","E6B4B1","E6B4B2","E6B4BC",
    "E6B581","E6B588","E6B58E","E6B58F","E6B591","E6B59A","E6B5A0","E6B5A6","E6B5A9","E6B5AA",
    "E6B5AE","E6B5B7","E6B682","E6B689","E6B69E","E6B69F","E6B6A0","E6B6A1","E6B6A6","E6B6A7",
    "E6B6AA","E6B6B5","E6B6BF","E6B780","E6B784","E6B785","E6B787","E6B7AE","E6B7B1","E6B7B3",
    "E6B885","E6B891","E6B89D","E6B8A0","E6B8A1","E6B8A9","E6B8AD","E6B8AF","E6B8B8","E6B984",
    "E6B996","E6B998","E6B99B","E6B99F","E6B9BE","E6BA86","E6BA90","E6BAA7","E6BAAA","E6BB81",
    "E6BB8B","E6BB91","E6BB95","E6BBA1","E6BBA6","E6BBA8","E6BBA9","E6BCA0","E6BCAF","E6BCB3",
    "E6BCBE","E6BD8D","E6BD98","E6BD9C","E6BD9E","E6BDA2","E6BDAD","E6BDAE","E6BDBC","E6BE84",
    "E6BE9C","E6BEA7","E6BEB3","E6BF89","E6BF9E","E6BFAE","E7818C","E781AF","E781B5","E78289",
    "E7828E","E782AE","E7839F","E783A6","E783BD","E78489","E784A6","E7858C","E785A7","E7869F",
    "E788B1","E7898C","E78999","E7899B","E7899F","E789A1","E789A7","E789B9","E78A81","E78A8D",
    "E78AB9","E78BAC","E78BAE","E78C97","E78CAE","E78E89","E78E8B","E78E9B","E78EAF","E78F99",
    "E78FA0","E78FAD","E78FB2","E79086","E790BC","E7919E","E792A7","E7939C","E793A6","E793AE",
    "E793AF","E79498","E794B0","E794B3","E794B5","E794B8","E7958C","E79599","E795A5","E795AA",
    "E795B4","E7968F","E799BB","E799BD","E799BE","E79A87","E79A8B","E79AAE","E79B82","E79B88",
    "E79B8A","E79B90","E79B91","E79B96","E79B98","E79B9B","E79B9F","E79BB1","E79C89","E79C99",
    "E79C9F","E79DA2","E79FB3","E7A080","E7A094","E7A09A","E7A195","E7A1AE","E7A28C","E7A291",
    "E7A29A","E7A2B1","E7A381","E7A390","E7A3B4","E7A4BC","E7A4BE","E7A581","E7A59D","E7A59E",
    "E7A5A5","E7A5A8","E7A684","E7A68F","E7A6B9","E7A6BA","E7A6BB","E7A6BE","E7A780","E7A789",
    "E7A791","E7A7A6","E7A7AD","E7A7AF","E7A7B0","E7A8B7","E7A8BB","E7A986","E7A997","E7A9B4",
    "E7AA81","E7AB99","E7ABA0","E7ABB9","E7AD89","E7AD96","E7ADA0","E7AE80","E7AEAD","E7B1B3",
    "E7B1BB","E7B2BE","E7B4A0","E7B4A2","E7B4AB","E7B6A6","E7B981","E7BAA2","E7BAB3","E7BB87",
    "E7BB8D","E7BB8F","E7BB93","E7BB9B","E7BBA5","E7BBA9","E7BBB4","E7BBB5","E7BBBF","E7BC99",
    "E7BD95","E7BD97","E7BE8C","E7BE8E","E7BF81","E7BF94","E7BFBC","E88080","E88081","E88083",
    "E88086","E88092","E880BF","E88182","E8818A","E88283","E88287","E882A5","E8839C","E883A1",
    "E883B6","E88482","E884B1","E8858A","E885BE","E887AA","E887B3","E88886","E8888D","E88892",
    "E8889E","E8889F","E889AF","E889B2","E88A82","E88A92","E88A9C","E88A9D","E88AA6","E88AAC",
    "E88AAE","E88AB1","E88AB7","E88B8D","E88B8F","E88B91","E88B97","E88BA5","E88BB1","E88C82",
    "E88C83","E88C85","E88C8C","E88CAB","E88CB6","E88D83","E88D86","E88D89","E88D94","E88DA3",
    "E88DA5","E88DAB","E88E86","E88E8E","E88E92","E88E98","E88E9E","E88EAB","E88EB1","E88EB2",
    "E88EB7","E88F8F","E8908D","E8909D","E890A5","E890A7","E890A8","E8919B","E891AB","E89297",
    "E89299","E892B2","E8939D","E8939F","E893A5","E893AC","E8949A","E894A1","E894BA","E89589",
    "E895B2","E895B4","E8969B","E89781","E897A4","E8998E","E8999E","E89A8C","E89B9F","E89E8D",
    "E8A0A1","E8A18C","E8A197","E8A1A1","E8A1A2","E8A395","E8A584","E8A5BF","E8A681","E8A789",
    "E8AEB7","E8AEB8","E8AF8F","E8AF95","E8AFB8","E8AFBA","E8B083","E8B08A","E8B08B","E8B09F",
    "E8B0A2","E8B0A6","E8B0B7","E8B1A1","E8B1AB","E8B49E","E8B4A1","E8B4A4","E8B4B5","E8B4B9",
    "E8B4BA","E8B584","E8B589","E8B596","E8B59B","E8B59E","E8B5A3","E8B5A4","E8B5AB","E8B5B5",
    "E8B5B7","E8B68A","E8B6B3","E8B7AF","E8BDA6","E8BDAE","E8BDBD","E8BE89","E8BE9B","E8BEB0",
    "E8BEB9","E8BEBD","E8BEBE","E8BF81","E8BF88","E8BF90","E8BF9B","E8BF9C","E8BF9E","E8BFA6",
    "E8BFAD","E9808A","E9809A","E98182","E98193","E981A5","E981B5","E98291","E98293","E98295",
    "E98297","E9829B","E982A1","E982A2","E982A3","E982AE","E982AF","E982B1","E982B3","E982B5",
    "E982B9","E982BB","E98381","E9838A","E9838E","E9838F","E98391","E98393","E983A7","E983A8",
    "E983AB","E983AD","E983AF","E983B4","E983B8","E983BD","E98482","E98484","E9849E","E984A2",
    "E984AF","E984B1","E98589","E98592","E986B4","E9878C","E9878D","E9878E","E98791","E99293",
    "E9929F","E992A2","E992A6","E99381","E99385","E9939C","E993B6","E99499","E994A1","E994A6",
    "E99587","E995B6","E995BF","E997A8","E997B4","E997B5","E997BB","E997BD","E99881","E99886",
    "E9989C","E998A1","E998B2","E998B3","E998B4","E998BF","E99980","E99982","E99984","E99986",
    "E99987","E99988","E99989","E99995","E9999F","E999B5","E999B6","E99A85","E99A86","E99A8F",
    "E99AB0","E99B84","E99B85","E99B86","E99B8D","E99BB7","E99C84","E99C8D","E99C9E","E99CB8",
    "E99D92","E99D96","E99D99","E99DA9","E99E8D","E99FA9","E99FB3","E99FB6","E9A1B6","E9A1B9",
    "E9A1BA","E9A28D","E9A29D","E9A38E","E9A5B6","E9A686","E9A696","E9A699","E9A9AC","E9A9BB",
    "E9A9BF","E9AA85","E9AB98","E9AD8F","E9B1BC","E9B281","E9B8A1","E9B8A3","E9B8AD","E9B9A4",
    "E9B9B0","E9B9BF","E9BA9F","E9BAA6","E9BABB","E9BB84","E9BB8E","E9BB91","E9BB94","E9BB9F",
    "E9BC8E","E9BC93","E9BD90","E9BE99",
};

unsigned char _font48_data[1][7]={
    "C2B0"
};

unsigned int _font14_width[96]={
    0x04,0x04,0x06,0x09,0x08,0x0C,0x0A,0x03,
    0x04,0x04,0x08,0x08,0x04,0x05,0x04,0x05,
    0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
    0x08,0x08,0x04,0x04,0x08,0x08,0x08,0x06,
    0x0C,0x09,0x09,0x08,0x0A,0x07,0x07,0x0A,
    0x0A,0x05,0x04,0x08,0x07,0x0C,0x0A,0x0B,
    0x08,0x0B,0x08,0x07,0x07,0x0A,0x08,0x0D,
    0x08,0x07,0x08,0x04,0x05,0x04,0x07,0x06,
    0x08,0x07,0x08,0x07,0x08,0x08,0x05,0x07,
    0x08,0x04,0x04,0x07,0x04,0x0D,0x08,0x08,
    0x08,0x08,0x05,0x07,0x05,0x08,0x07,0x0A,
    0x07,0x07,0x07,0x05,0x07,0x05,0x08,0x0E
 };
unsigned int _font16_width[96]={
    0x04,0x04,0x07,0x0A,0x09,0x0D,0x0B,0x04,//32-39
    0x05,0x05,0x09,0x09,0x04,0x05,0x04,0x06,//40-47
    0x09,0x09,0x09,0x09,0x09,0x09,0x09,0x09,//48-55
    0x09,0x09,0x04,0x04,0x09,0x09,0x09,0x07,//56-63
    0x0E,0x0B,0x0B,0x0A,0x0B,0x08,0x08,0x0B,//64-71
    0x0B,0x06,0x04,0x09,0x08,0x0E,0x0C,0x0B,//72-79
    0x09,0x0B,0x0A,0x08,0x08,0x0B,0x0A,0x0E,//80-87
    0x09,0x09,0x09,0x05,0x06,0x05,0x09,0x07,//88-95
    0x09,0x09,0x09,0x08,0x09,0x09,0x05,0x08,//95-103
    0x09,0x04,0x04,0x08,0x04,0x0E,0x09,0x09,//104-111
    0x09,0x09,0x06,0x07,0x05,0x09,0x08,0x0C,//112-119
    0x08,0x08,0x08,0x06,0x09,0x06,0x09,0x10 //120-127
};

GFX_RESOURCE_HDR *mcu_font[12]={
   (GFX_RESOURCE_HDR *)&font14,
   (GFX_RESOURCE_HDR *)&font16,
   (GFX_RESOURCE_HDR *)&font18,
   (GFX_RESOURCE_HDR *)&city18,
   (GFX_RESOURCE_HDR *)&weather18,
   (GFX_RESOURCE_HDR *)&font20,
   (GFX_RESOURCE_HDR *)&font22,
   (GFX_RESOURCE_HDR *)&alert_font24,
   (GFX_RESOURCE_HDR *)&font26,
   (GFX_RESOURCE_HDR *)&font_pf42,
   (GFX_RESOURCE_HDR *)&font48,
   
};

GFX_RESOURCE_HDR *mcu_bitmap[]={
    (GFX_RESOURCE_HDR *)&icon_default,
    (GFX_RESOURCE_HDR *)&icon_1jia,
    (GFX_RESOURCE_HDR *)&icon_360,
    (GFX_RESOURCE_HDR *)&icon_asus,
    (GFX_RESOURCE_HDR *)&icon_coolpad,
    (GFX_RESOURCE_HDR *)&icon_dell,
    (GFX_RESOURCE_HDR *)&icon_haier,
    (GFX_RESOURCE_HDR *)&icon_hasee,
    (GFX_RESOURCE_HDR *)&icon_honor,
    (GFX_RESOURCE_HDR *)&icon_hp,
    (GFX_RESOURCE_HDR *)&icon_htc,
    (GFX_RESOURCE_HDR *)&icon_huawei,
    (GFX_RESOURCE_HDR *)&icon_iPhone,
    (GFX_RESOURCE_HDR *)&icon_lenovo,
    (GFX_RESOURCE_HDR *)&icon_letv,
    (GFX_RESOURCE_HDR *)&icon_lg,
    (GFX_RESOURCE_HDR *)&icon_meitu,
    (GFX_RESOURCE_HDR *)&icon_meizu,
    (GFX_RESOURCE_HDR *)&icon_oppo,
    (GFX_RESOURCE_HDR *)&icon_phicomm,
    (GFX_RESOURCE_HDR *)&icon_samsung,
    (GFX_RESOURCE_HDR *)&icon_smartisan,
    (GFX_RESOURCE_HDR *)&icon_sony,
    (GFX_RESOURCE_HDR *)&icon_tcl,
    (GFX_RESOURCE_HDR *)&icon_thinkpad,
    (GFX_RESOURCE_HDR *)&icon_tongfang,
    (GFX_RESOURCE_HDR *)&icon_vivo,
    (GFX_RESOURCE_HDR *)&icon_windowsphone,
    (GFX_RESOURCE_HDR *)&icon_xiaomi,
    (GFX_RESOURCE_HDR *)&icon_zte,
    (GFX_RESOURCE_HDR *)&w_locationfailed,
    (GFX_RESOURCE_HDR *)&w_cloudy,
    (GFX_RESOURCE_HDR *)&w_dust,
    (GFX_RESOURCE_HDR *)&w_foggy,
    (GFX_RESOURCE_HDR *)&w_haze,
    (GFX_RESOURCE_HDR *)&w_overcast,
    (GFX_RESOURCE_HDR *)&w_rain,
    (GFX_RESOURCE_HDR *)&w_snow,
    (GFX_RESOURCE_HDR *)&w_sun,
    (GFX_RESOURCE_HDR *)&w_windy,
    (GFX_RESOURCE_HDR *)&w_unknown,
    (GFX_RESOURCE_HDR *)&phicomm,
    (GFX_RESOURCE_HDR *)&icon_update,
    (GFX_RESOURCE_HDR *)&icon_two_dimensional_code,
    (GFX_RESOURCE_HDR *)&icon_hint_disconnected,
    (GFX_RESOURCE_HDR *)&icon_upload_white,
    (GFX_RESOURCE_HDR *)&icon_download_white,
    (GFX_RESOURCE_HDR *)&icon_wifi,
    (GFX_RESOURCE_HDR *)&icon_visitor,
    (GFX_RESOURCE_HDR *)&icon_information_background_double,
    (GFX_RESOURCE_HDR *)&icon_information_background_single,
    (GFX_RESOURCE_HDR *)&icon_usb_link,
    (GFX_RESOURCE_HDR *)&icon_apmode,
};

void MCU_SetColor(MCU_COLOR c)
{
    unsigned char b = 0x00;
    unsigned char g = 0x00;
    unsigned char r = 0x00;
    b = 0xFF & (c>>16);
    g = 0xFF & (c>>8);
    r = 0xFF & c;
    GFX_ColorSet( GFX_INDEX_0, GFX_RGBConvert( r, g, b) );
}
void MCU_SetFont(unsigned int font_index)
{
    _cur_font_index = font_index;
    GFX_FontSet( GFX_INDEX_0, mcu_font[font_index]); // set font
}
void MCU_SetTextAlign(MCU_TEXTALIGN ta)
{
    ;
}
void MCU_FillRect(MCU_RECT r)
{
    GFX_RectangleFillDraw( GFX_INDEX_0, r.x0, r.y0, r.x1, r.y1);
}
void MCU_SetTextMode(MCU_TEXTMODE tm)
{
   ;
}
void MCU_SetWrapMode(MCU_WRAPMODE wm)
{
    ;
}

void MCU_DrawTextAtPos(const unsigned char *str,unsigned int x,unsigned int y)
{
    set_gfxxchar_value((unsigned char *)str);
    GFX_TextStringDraw( GFX_INDEX_0, x, y, _tmp_str, 0);
}

void MCU_DrawText(const unsigned char *str)
{
    MCU_DrawTextAtPos( str, 0, 0);
}

void MCU_DrawTextInRect(const unsigned char *str ,MCU_RECT rect, MCU_TEXTALIGN ta, MCU_TEXTMODE tm, MCU_WRAPMODE wm)
{
    int i=0;
    int str_char_width =0;
    unsigned char tmp[64]={0x0};
    int rect_width = 0;
    rect_width = rect.y1-rect.y0;
    if ( 2 == wm ) //MCU_WRAPMODE_CHAR
    {
       
       if (0 == _cur_font_index)//MCU_FONT_WQY14
       {
           for (i=0;i<strlen(str);i++)
           {
               str_char_width += MCU_GetCharWidthByFontSize(_cur_font_index,str[i]);
               if (str_char_width > (rect.x1-rect.x0) )
               {
                   break;
               }
           }
           if ( i < strlen(str) )
           {
                strncpy( tmp, str, i);
                tmp[i+1]=0x0;
                set_gfxxchar_value(tmp);
                GFX_TextStringBoxDraw( GFX_INDEX_0, rect.x0, rect.y0, rect.x1-rect.x0, rect_width/2, _tmp_str, 0, ta);
                strcpy( tmp, str+i);
                set_gfxxchar_value(tmp);
                GFX_TextStringBoxDraw( GFX_INDEX_0, rect.x0, rect.y0+rect_width/2, rect.x1-rect.x0, rect_width/2, _tmp_str, 0, ta);
                return ;
           }
       }
    }
   
    set_gfxxchar_value((char *)str);
    GFX_TextStringBoxDraw( GFX_INDEX_0, rect.x0, rect.y0, rect.x1-rect.x0, rect.y1-rect.y0, _tmp_str, 0, ta);
  
  
}
void MCU_SetPenSize(unsigned char psize)
{
    _cur_pen_size = psize;
}

void MCU_DrawLine(MCU_LINE l)
{
    unsigned char i = 0;
    for ( i=0; i< _cur_pen_size; ++i){
        GFX_LineDraw( GFX_INDEX_0, l.x0, l.y0+i, l.x1, l.y1+i);
    }
    _cur_pen_size = 1;
}

void MCU_DrawBitmap(unsigned int bmp_index)
{
    MCU_DrawBitmapAtPos(bmp_index,0,0);
}

void MCU_DrawBitmapAtPos(unsigned int bmp_index,unsigned int x,unsigned int y)
{ 
    GFX_ImageDraw( GFX_INDEX_0, x, y, mcu_bitmap[bmp_index]);
}
/*
 * 获取当前字符的点阵宽度
 */
unsigned int MCU_GetCharWidthByFontSize(unsigned int font_index,unsigned char c)
{
    switch(font_index)
    {
        case 0://MCU_FONT_WQY14
            return _font14_width[c-32];
            break;
        case 1://MCU_FONT_WQY16
            return _font16_width[c-32];
            break;
    }
    
}

void MCU_AA_DrawLine(MCU_LINE l)
{
   MCU_DrawLine(l);
}
void MCU_AA_SetFactor(unsigned int f)
{
   ;
}
void MCU_DrawRect(MCU_RECT r)
{
    GFX_RectangleDraw(GFX_INDEX_0,r.x0,r.y0,r.x1,r.y1);
}

void MCU_Clear()
{
    MCU_RECT rect = {0,0,320,440};
    MCU_SetColor(0x00000000);
    MCU_FillRect(rect);
   // GFX_ColorSet(GFX_INDEX_0, GFX_RGBConvert(0x00,0x00,0x00));
   // GFX_ScreenClear(GFX_INDEX_0);
}
/*
 get cur font  Chinese num
 */
unsigned int get_font_data_num(unsigned int font_index)
{
    unsigned int font_num = 0;
    switch(font_index)
    {
        case 0://MCU_FONT_WQY14
            font_num = (sizeof(_font14_data)/sizeof(unsigned char )) / (sizeof(_font14_data[0])/sizeof(unsigned char)); //24
            break;
        case 1://MCU_FONT_WQY16
            font_num = (sizeof(_font16_data)/sizeof(unsigned char )) / (sizeof(_font16_data[0])/sizeof(unsigned char)); //46;
            break;
        case 2://MCU_FONT_WQY18:
            font_num = (sizeof(_font18_data)/sizeof(unsigned char )) / (sizeof(_font18_data[0])/sizeof(unsigned char)); // 20
            break;
        case 3://MCU_CITY_FONT_WQY18:
            font_num = (sizeof(_font18_data_city)/sizeof(unsigned char )) / (sizeof(_font18_data_city[0])/sizeof(unsigned char)); //1114
            break;
        case 4://MCU_WEATHER_WQY18:
            font_num = (sizeof(_font18_data_weather)/sizeof(unsigned char )) / (sizeof(_font18_data_weather[0])/sizeof(unsigned char)); //35
            break;
        case 5://MCU_FONT_WQY20:
            font_num = (sizeof(_font20_data)/sizeof(unsigned char )) / (sizeof(_font20_data[0])/sizeof(unsigned char)); // 13
            break;
        case 6://MCU_FONT_WQY22:
            font_num = (sizeof(_font22_data)/sizeof(unsigned char )) / (sizeof(_font22_data[0])/sizeof(unsigned char)); //7
            break;
        case 7://MCU_FONT_WQY24:
            font_num = (sizeof(_font24_data)/sizeof(unsigned char )) / (sizeof(_font24_data[0])/sizeof(unsigned char)); //28
            break;
        case 8://MCU_FONT_WQY26:
            font_num = (sizeof(_font26_data)/sizeof(unsigned char )) / (sizeof(_font26_data[0])/sizeof(unsigned char)); //43
            break;
        case 9:
            font_num = 0; 
            break;
        case 10:
            font_num = 1;
    }
    return font_num;
}
void set_gfxxchar_value_by_font_data(unsigned char font_data[][7],unsigned char *str)
{
	int s = 0;
	int e = 0;
	int m = 0;
    int i = 0,j = 0;
	int font_num = 0;
	unsigned char tmp[7]={0};
	memset(_tmp_str,0x0,sizeof(GFX_XCHAR)*64);
	font_num = get_font_data_num(_cur_font_index);

	e = font_num;
	for (i=0;i<strlen(str);i++)
	{
		memset(tmp,0x0,sizeof(tmp));
		if (0x00e0 == (str[i] & 0x00e0))
		{
			sprintf(tmp,"%02X%02X%02X",str[i],str[i+1],str[i+2]);
			i+=2;
		}else if (0x00c0 == (str[i] & 0x00c0))
		{
			sprintf(tmp,"%02X%02X",str[i],str[i+1]);
			i+=1;
		}else if (str[i]<0x7f)
		{
			_tmp_str[j++] = str[i];
			continue;
		}
		s = 0;
		e = font_num;
		m = (s+e)/2;
		while (strcmp(font_data[m],tmp)!=0 &&  s!=e)
		{
			if (strcmp(font_data[m],tmp)>0)
			{
				if( e!=m)
				{
					e = m;
				}
				else
				{
					break;
				}
			}
			else if( strcmp(font_data[m],tmp)<0) // else
			{
				if (s!=m)
				{
					s = m;
				}
				else
				{
					break;
				}
			}
			m = (s+e)/2;
		}
		if ( 0 == strcmp(font_data[m],tmp))
        {
            _tmp_str[j++] = m +128;
        }else if ( (m-1) > 0 && 0 == strcmp(font_data[m-1],tmp))
        {
            _tmp_str[j++] = m-1+128;
        }
        else if( (m+1) > font_num && 0 == strcmp(font_data[m+1],tmp))
        {
            _tmp_str[j++] = m+1+128;
        }
	}
	_tmp_str[j]=0x0;
}

void set_gfxxchar_value( unsigned char *str)
{
    switch(_cur_font_index)
    {
        case 0://MCU_FONT_WQY14
            set_gfxxchar_value_by_font_data( _font14_data, str);
            break;
        case 1://MCU_FONT_WQY16
            set_gfxxchar_value_by_font_data( _font16_data, str);
            break;
        case 2://MCU_FONT_WQY18
            set_gfxxchar_value_by_font_data( _font18_data, str);
            break;
        case 3://MCU_CITY_FONT_WQY18
            set_gfxxchar_value_by_font_data( _font18_data_city, str);
            break;
        case 4://MCU_WEATHER_WQY18
            set_gfxxchar_value_by_font_data( _font18_data_weather, str);
            break;
        case 5://MCU_FONT_WQY20
            set_gfxxchar_value_by_font_data( _font20_data, str);
            break;
        case 6://MCU_FONT_WQY22
            set_gfxxchar_value_by_font_data( _font22_data, str);
            break;
        case 7://MCU_FONT_WQY24
            set_gfxxchar_value_by_font_data( _font24_data, str);
            break;
        case 8://MCU_FONT_WQY26
            set_gfxxchar_value_by_font_data( _font26_data, str);
            break;
        case 9://MCU_FONT_PF42
            set_gfxxchar_value_by_font_data( NULL, str);
            break;
        case 10:
            set_gfxxchar_value_by_font_data( _font48_data, str);
            break;
            
    }
}