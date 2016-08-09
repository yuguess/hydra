function updateRow(id,data) {
  var trade_quantity = Number($('#' + id + "_trade_quantity").text()) + Number(data.trade_quantity);
  var tradeString = "" + trade_quantity + "/" + $('#' + id + "_quantity").text();
  var percentage=100 * trade_quantity/Number($('#'+id+"_quantity").text());

  $('#' + id + "_status").html(error_code[data.error_code]);
  $('#' + id + "_trade_quantity").html(trade_quantity)
  $('#' + id + "_progress").css({'width':"" + percentage + "%"}).find('span').html(tradeString);

  changecolor(id,data);
}

function changecolor(id,data) {
  changeStatusColor(id,data.error_code);
}

function changeStatusColor(id, error_code) {
  switch (error_code) {
    case 0:
      $('#'+id+"_status").css("background-color","blue");
      break;
    case 1:
      $('#'+id+"_status").css("background-color","red");
      break;
    case 2:
      $('#'+id+"_status").css("background-color","red");
      break;
    case 3:
      $('#'+id+"_status").css("background-color","red");
      break;
    case 4:
      $('#'+id+"_status").css("background-color","yellow");
      break;
    case 5:
      $('#'+id+"_status").css("background-color","grey");
      break;
    case 6:
      $('#'+id+"_status").css("background-color","grey");
      break;
    case 7:
      $('#'+id+"_status").css("background-color","blue");
      break;
  }
}

function newOrder(data) {
  $("#title").after(   "<tr id='" + data.id + "'> \
                            <td id='" + data.id + "_status'>" + "new order" + "</td> \
                            <td id='" + data.id + "_code'>" + data.code + "</td> \
                            <td id='" + data.id + "_price'>" + Math.round(data.limit_price * 1000) / 1000 + "</td> \
                            <td id='" + data.id + "_buy_sell'>" + data.buy_sell + "</td> \
                            <td style=\"display:none\" id='" + data.id + "_trade_quantity'>" + "0" + "</td> \
                            <td style=\"display:none\" id='" + data.id + "_quantity'>" + data.trade_quantity + "</td> \
                            <td> \
                               <div class=\"progress\"> \
                                  <div class=\"progress-bar\" id='" + data.id + "_progress' role=\"progressbar\" aria-valuenow=\"0\" aria-valuemin=\"0\"  \
                                  aria-valuemax=\"100\" style=\"width:100%\"> \
                                      <span>0</span>\
                                  </div> \
                              </div>\
                            </td> \
                            <td id='" + data.id + "_open_close'>" + data.open_close + "</td> \
                            <td id='" + data.id + "_account'>" + data.account + "</td> \
                            <td id='" + data.id + "_argument_list'>" + data.argument_list + "</td> \
                            <td id='" + data.id + "_id'>" + map[count.toString()] + "</td> \
                            <td id='" + data.id + "_status_msg'>" + "" + "</td> \
                          </tr>"); 
  $('#'+data.id+"_progress").find('span').css("color","black");
  if (data.buy_sell == "SHORT_SELL") {
    $('#'+data.id+'_buy_sell').css("background-color","green");
  } else if (data.buy_sell == "LONG_BUY") {
    $('#'+data.id+'_buy_sell').css("background-color","red");
  }
}

var error_code={
  '0': "SEND_CONFIRM",
  '1': "SEND_ERROR",
  '2': "CANCEL_ERROR",
  '3': 'BAD_ORDER_ERROR',
  '4': 'DEAL',
  '5': "CANCELED",
  '6': "PARTIALLY_CANCELLED",
  '7': "PARTIALLY_DEAL",
};

var map = new Map();
var invMap = new Map();
var count = 0;
var ws = new WebSocket('ws://192.168.0.66:8000/soc');

ws.onmessage = function(event) {
  try {
    var jData = JSON.parse(event.data);
    var data = JSON.parse(jData.message);
  } catch (e) {
    alert(e.message);
  }
  
  
  if (data.cedar_msg_type == "TYPE_ORDER_REQUEST" && data.type != "TYPE_CANCEL_ORDER_REQUEST" ) {

    if (invMap[data.id] != undefined) return;
    
    map[count.toString()] = data.id;
    invMap[data.id] = count.toString();
    data.id = count.toString();
    
    newOrder(data);
    
    count +=1;
  } else {
    data.ref_id = invMap[data.ref_id];
    
    if (data.ref_id == undefined) {
      document.getElementById('debug').insertRow().insertCell().innerHTML = jData.message;
      return;
    }

    updateRow(data.ref_id, data)
  }
}
