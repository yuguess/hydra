function showStock() {
  $('#future_div').hide();
  $('#stock_div').show();
}

function showFuture() {
  $('#future_div').show();
  $('#stock_div').hide();
}



//date formating
Date.prototype.format =function(format) {
        var o = {
        "M+" : this.getMonth()+1, //month
"d+" : this.getDate(),    //day
"h+" : this.getHours(),   //hour
"m+" : this.getMinutes(), //minute
"s+" : this.getSeconds(), //second
"q+" : Math.floor((this.getMonth()+3)/3),  //quarter
"S" : this.getMilliseconds() //millisecond
        }
        if(/(y+)/.test(format)) format=format.replace(RegExp.$1,
        (this.getFullYear()+"").substr(4- RegExp.$1.length));
        for(var k in o)if(new RegExp("("+ k +")").test(format))
        format = format.replace(RegExp.$1,
        RegExp.$1.length==1? o[k] :
        ("00"+ o[k]).substr((""+ o[k]).length));
        return format;
}



//更改第二张表algo的reference price
//change the reference price for specific algo row
function updateAppStatus(data) {
  $("#"+invMapAlgo[data.alg_order_id]+"_ref_price").html(data.values);
 // document.getElementById('debug').insertRow().insertCell().innerHTML = "#"+data.alg_order_id+"_ref_price";
  //document.getElementById('debug').insertRow().insertCell().innerHTML = data.values;
}

//当出现app_status先于order到达时，创建algo对应的行
//when app_status comes befor order information, create a algo row
function constructAlgoFromAppStatus(data) {
  if (invMapAlgo[data.alg_order_id] != undefined) return;
  mapAlgo[countAlgo.toString()] = data.alg_order_id;
  invMapAlgo[data.alg_order_id] = countAlgo.toString();
  data.alg_order_id = countAlgo.toString();

  $("#algo-title").after("<tr id='" + data.alg_order_id + "' data-toggle=\"collapse\" href=\"#" + data.alg_order_id +"_collapse\" onclick = \"onClickAlgo(this);\" > \
                            <td id='" + data.alg_order_id + "_algo_id'>" + mapAlgo[data.alg_order_id] + "</td> \
                            <td id='" + data.alg_order_id + "_ref_price'>" + data.values + "</td> \
                            <td id='" + data.alg_order_id + "_trade_price'>" + " " + "</td> \
                            <td id='" + data.alg_order_id + "_slippage'>" + " " + "</td> \
                            <td style=\"display:none\" id='" + data.alg_order_id + "_algo_trade_quantity'>" + "0" + "</td> \
                            <td style=\"display:none\" id='" + data.alg_order_id + "_algo_quantity'>" + 0 + "</td> \
                            <td> \
                              <div class=\"progress\"> \
                                  <div class=\"progress-bar\" id='" + data.alg_order_id + "_algo_progress' role=\"progressbar\" aria-valuenow=\"0\" aria-valuemin=\"0\"  \
                                  aria-valuemax=\"100\" style=\"width:0%\"> \
                                      <span style=\"color:black\">0</span>\
                                  </div> \
                              </div>\
                            </td> \
                          </tr>"); 
  countAlgo +=1;
}

//在首次连接时，table信息到来的时候，创建order行
//when connect first time, and table info comes, create a order row
function constructOrder(data) {
  date = new Date(0);
  s = mapOrder[data.id]
  date.setUTCSeconds(Number(s.substring(0,s.length-6)));
  var sdate = date.format("hh:mm:ss");  

  var percentage= Number(data.trade_quantity)/Number(data.quantity);
  $("#order-title").after("<tr class='order' id='" + data.id + "' account='" + data.account + "'> \
                            <td id='" + data.id + "_status'>" + data.status + "</td> \
                            <td id='" + data.id + "_time'>" + sdate + "</td> \
                            <td id='" + data.id + "_code'>" + data.code + "</td> \
                            <td id='" + data.id + "_price'>" + data.price + "</td> \
                            <td style=\"display:none\" id='" + data.id + "_order_trade_quantity'>" + data.trade_quantity + "</td> \
                            <td style=\"display:none\" id='" + data.id + "_order_quantity'>" + data.quantity + "</td> \
                            <td style=\"display:none\" id='" + data.id + "_batch_flag'>" + data.batch_flag + "</td> \
                            <td style=\"display:none\" id='" + data.id + "_algo_flag'>" + data.algo_flag + "</td> \
                            <td> \
                               <div class=\"progress\"> \
                                  <div class=\"progress-bar\" id='" + data.id + "_order_progress' role=\"progressbar\" aria-valuenow=\"0\" aria-valuemin=\"0\"  \
                                  aria-valuemax=\"100\" style=\"width:" + 100*percentage + "%\"> \
                                      <span style=\"color:black\">" + data.trade_quantity + '/' + data.quantity + "</span>\
                                  </div> \
                              </div>\
                            </td> \
                            <td id='" + data.id + "_open_close'>" + data.open_close + "</td> \
                            <td id='" + data.id + "_accountOrder'>" + data.account + "</td> \
                            <td id='" + data.id + "_buy_sell'>" + data.buy_sell + "</td> \
                            <td id='" + data.id + "_argument_list'>" + data.argument_list + "</td> \
                            <td id='" + data.id + "_id'>" + mapOrder[countOrder.toString()] + "</td> \
                          </tr>"); 
  $('#'+data.id+"_progress").find('span').css("color","black");
  if (data.buy_sell == "SHORT_SELL") {
    $('#'+data.id+'_buy_sell').attr("class","danger");
  } else if (data.buy_sell == "LONG_BUY") {
    $('#'+data.id+'_buy_sell').attr("class","info");
  }
  changeStatusColor(data.id,data.status)
}

//在首次连接时，table信息到来的时候，创建algo行
//when connect first time, and table info comes, create a algo row
function constructAlgo(data) {
  $("#algo-title").after("<tr id='" + data.algo_flag + "' data-toggle=\"collapse\" href=\"#" + data.algo_flag +"_collapse\" onclick = \"onClickAlgo(this);\" account='" + data.account + "'> \
                            <td id='" + data.algo_flag + "_algo_id'>" + mapAlgo[data.algo_flag] + "</td> \
                            <td id='" + data.algo_flag + "_ref_price'>" + " " + "</td> \
                            <td id='" + data.algo_flag + "_trade_price'>" + " " + "</td> \
                            <td id='" + data.algo_flag + "_slippage'>" + " " + "</td> \
                            <td style=\"display:none\" id='" + data.algo_flag + "_algo_trade_quantity'>" + "0" + "</td> \
                            <td style=\"display:none\" id='" + data.algo_flag + "_algo_quantity'>" + data.trade_quantity + "</td> \
                            <td> \
                              <div class=\"progress\"> \
                                  <div class=\"progress-bar\" id='" + data.algo_flag + "_algo_progress' role=\"progressbar\" aria-valuenow=\"0\" aria-valuemin=\"0\"  \
                                  aria-valuemax=\"100\" style=\"width:0%\"> \
                                      <span style=\"color:black\">0</span>\
                                  </div> \
                              </div>\
                            </td> \
                          </tr>"); 
}

//在首次连接时，table信息到来的时候，创建batch行
//when connect first time, and table info comes, create a batch row
function constructBatch(data) {
  $("#batch-title").after("<tr id='" + data.batch_flag + "' onclick = \"onClickBatch(this);\" account='" + data.account + "'> \
                          <td id='" + data.batch_flag + "_algo_id'>" + mapBatch[data.batch_flag] + "</td> \
                          <td style=\"display:none\" id='" + data.batch_flag + "_batch_trade_quantity'>" + "0" + "</td> \
                          <td style=\"display:none\" id='" + data.batch_flag + "_batch_quantity'>" + data.trade_quantity + "</td> \
                          <td> \
                             <div class=\"progress\"> \
                                <div class=\"progress-bar\" id='" + data.batch_flag + "_batch_progress' role=\"progressbar\" aria-valuenow=\"0\" aria-valuemin=\"0\"  \
                                aria-valuemax=\"100\" style=\"width:0%\"> \
                                    <span style=\"color:black\">0</span>\
                                </div> \
                            </div>\
                          </td> \
                        </tr>"); 
}


//在首次连接时，table信息到来的时候，处理信息
//when connect first time, and table info comes, process 
function constructOrderTable(data) {
    if (invMapOrder[data.id] != undefined) return;
    mapOrder[countOrder.toString()] = data.id;
    invMapOrder[data.id] = countOrder.toString();
    data.id = countOrder.toString();
    
    constructOrder(data);
    //sumBatchProgress(data.batch_flag);
    //sumAlgoProgress(data.algo_flag);
    countOrder +=1;
}

//在首次连接时，table信息到来的时候，处理信息
//when connect first time, and table info comes, process 
function constructAlgoTable(data) {
  if (invMapAlgo[data.algo_flag] != undefined) return;
  mapAlgo[countAlgo.toString()] = data.algo_flag;
  invMapAlgo[data.algo_flag] = countAlgo.toString();
  data.algo_flag = countAlgo.toString();
  
  constructAlgo(data);
  
  countAlgo +=1;
}

//在首次连接时，table信息到来的时候，处理信息
//when connect first time, and table info comes, process 
function constructBatchTable(data) {
  if (invMapBatch[data.batch_flag] != undefined) return;
  
  mapBatch[countBatch.toString()] = data.batch_flag;
  invMapBatch[data.batch_flag] = countBatch.toString();
  data.batch_flag = countBatch.toString();
  
  constructBatch(data);
  
  countBatch +=1;
}

//点击batch的标题栏时，显示所有信息
//show all batch,algo,order info when clicking on the theme of the batch table
function onClickShowAll(data) {
    //$('#audio').play();
  //$("#audio")[0].play();

  $("#order tr").each(function() {
    jQuery(this).show();
  });

  $("#algo tr").each(function() {
    jQuery(this).show();
  });
}

//点击batch的标时，显示对应信息
//show proper rows, when clicking on batch row
function onClickBatch(data){
  var batch_value = jQuery(data).children(":first").html();

  tmp_algo =new Map();

  $("#order tr").each(function() {
    if (jQuery(this).attr("id")=='order-title') {
      return
    } else if (jQuery(this).children("[id$=_batch_flag]").html() != batch_value) {
      jQuery(this).hide();
    } else{
      var algo_name = jQuery(this).children("[id$=_algo_flag]").html()
      tmp_algo[algo_name] = '1';
      jQuery(this).show();
    };
    // document.getElementById('debug').insertRow().insertCell().innerHTML = algo_value;
  });

  $("#algo tr").each(function() {
    if (jQuery(this).attr("id")=='algo-title') {
      return
    } else if (tmp_algo[jQuery(this).children(":first").html()] == undefined) {
      jQuery(this).hide();
    } else {
      jQuery(this).show();
    }
  })
}

//点击algo的标时，显示对应信息
//show proper rows, when clicking on algo row
function onClickAlgo(data){
  var algo_value = jQuery(data).children(":first").html();
  $("#order tr").each(function() {
    if (jQuery(this).attr("id")=='order-title') {
      return;
    } else if (jQuery(this).children("[id$=_algo_flag]").html() != algo_value) {
      jQuery(this).hide();
    } else{
      jQuery(this).show();
    };

    // document.getElementById('debug').insertRow().insertCell().innerHTML = algo_value;
  });
}

//当有新的order update到来时，更改对应order行内容
//update order info when new order update comes
function updateOrderRow(id,data) {
  var trade_quantity = Number($('#' + id + "_order_trade_quantity").text()) + Number(data.trade_quantity);
  var tradeString = "" + trade_quantity + "/" + $('#' + id + "_order_quantity").text();
  var percentage=100 * trade_quantity/Number($('#'+id+"_order_quantity").text());

  $('#' + id + "_status").html(error_code[data.error_code]);
  $('#' + id + "_order_trade_quantity").html(trade_quantity);
  $('#' + id + "_order_progress").css({'width':"" + percentage + "%"}).find('span').html(tradeString);

  $("#audio source").attr("src","http://tsn.baidu.com/text2audio?tex=" + $('#' + id + "_code").html() + " 已成交" + "\&lan=zh&cuid=" + "94-DE-80-23-E5-A6" + "\&spd=9\&pit=6\&ctp=1&tok="+"24.e175ed83539ebe33d2eb67c61effe559.2592000.1475223151.282335-8572922");
  $("#audio")[0].pause();
  $("#audio")[0].load();
  $("#audio")[0].play();

  //document.getElementById('debug').insertRow().insertCell().innerHTML = $('#' + id + "_code").html();

  changecolor(id,data);
}

//当有新的order update到来时，更改对应batch行内容
//update batch info when new order update comes
function updateBatchRow(batch_id, data) {
 var batch_flag;
  $("td[id$='" + data.ref_id + "_batch_flag']").each(function() {
    batch_flag = this.innerHTML;
  });
  sumBatchProgress(batch_flag);
}

//计算batch进度
//compute batch progress
function sumBatchProgress(batch_flag) {
  //document.getElementById('debug').insertRow().insertCell().innerHTML = "exect";
  var trade_quantity=0;
  var quantity = 0;

  $("tr").filter(function(){
    var status = jQuery(this).children(":first").html();
    //document.getElementById('debug').insertRow().insertCell().innerHTML = algo_flag;
    return (jQuery(this).children("[id$=_batch_flag]").html() == batch_flag && jQuery(this).hasClass('order') && countable_states.indexOf(status) != -1);

  }).each(function() {

    s_quantity = jQuery(this).children("[id$=_order_quantity]").html();
    s_trade_quantity = jQuery(this).children("[id$=_order_trade_quantity]").html();
    trade_quantity += Number(s_trade_quantity);
    quantity += Number(s_quantity);

    //document.getElementById('debug').insertRow().insertCell().innerHTML = jQuery(this).parent().children("[id$=_order_quantity]").html();
  });

  var tradeString = "" + trade_quantity + "/" + quantity;
  var percentage=100 * trade_quantity/quantity;

  $('#' + invMapBatch[batch_flag] + "_batch_progress").css({'width':"" + percentage + "%"}).find('span').html(tradeString);

  //document.getElementById('debug').insertRow().insertCell().innerHTML = tradeString;

}

//当有新的order update到来时，更改对应algo行内容
//update algo info when new order update comes
function updateAlgoRow(algo_id, data) {
  var algo_flag;
  $("td[id$='" + data.ref_id + "_algo_flag']").each(function() {
    algo_flag = this.innerHTML;
  });
  sumAlgoProgress(algo_flag);
  computeTradePrice(algo_flag);
}

//计算algo进度
//compute algo progress
function sumAlgoProgress(algo_flag) {
  var trade_quantity=0;
  var quantity = 0;

  $("tr").filter(function(){
    var status = jQuery(this).children(":first").html();
    //document.getElementById('debug').insertRow().insertCell().innerHTML = algo_flag;
    return (jQuery(this).children("[id$=_algo_flag]").html() == algo_flag && jQuery(this).hasClass('order') && countable_states.indexOf(status) != -1);

  }).each(function() {

    s_quantity = jQuery(this).children("[id$=_order_quantity]").html();
    s_trade_quantity = jQuery(this).children("[id$=_order_trade_quantity]").html();
    trade_quantity += Number(s_trade_quantity);

    //document.getElementById('debug').insertRow().insertCell().innerHTML = jQuery(this).parent().children("[id$=_order_quantity]").html();
  });
  quantity = algo_quantity_map[algo_flag];
  var tradeString = "" + trade_quantity + "/" + quantity;
  var percentage=100 * trade_quantity/quantity;

  $('#' + invMapAlgo[algo_flag] + "_algo_progress").css({'width':"" + percentage + "%"}).find('span').html(tradeString);

  //document.getElementById('debug').insertRow().insertCell().innerHTML = tradeString;
}

//计算制定algo的交易价格
//compute the real trade price of specific algo
function computeTradePrice(algo_flag) {
  var notional=0;
  var quantity = 0;

  $("tr").filter(function() {
    var status = jQuery(this).children(":first").html();
    //document.getElementById('debug').insertRow().insertCell().innerHTML = status;
    return (jQuery(this).children("[id$=_algo_flag]").html() == algo_flag && jQuery(this).hasClass('order') && countable_states.indexOf(status) != -1);

  }).each(function() {

    s_quantity = jQuery(this).children("[id$=_order_trade_quantity]").html();
    s_notional = jQuery(this).children("[id$=_order_trade_quantity]").html()*jQuery(this).children("[id$=price]").html();
    quantity += Number(s_quantity);
    notional += Number(s_notional);

    //document.getElementById('debug').insertRow().insertCell().innerHTML = jQuery(this).children("[id$=_order_trade_quantity]").html();
  });
  
  var trade_price = notional/quantity;
  
  $('#' + invMapAlgo[algo_flag] + "_trade_price").html(trade_price);
  try{
    var direction = 0;
    var buy_sell;
    $('tr').children("[id$=_algo_flag]").filter(function() {
      if (jQuery(this).html() == algo_flag) {
        buy_sell = jQuery(this).parent().children('[id$=_buy_sell]').html();
        //document.getElementById('debug').insertRow().insertCell().innerHTML = buy_sell;
        if (buy_sell == "LONG_BUY") {
          direction = -1;
        } else {
          direction = 1;
        }
      }
    });

    //alert($("#"+invMapAlgo[algo_flag]+"_ref_price").html()+","); 
    var slippage = parseInt((trade_price/Number($("#"+invMapAlgo[algo_flag]+"_ref_price").html())-1)*direction*10000);
    //document.getElementById('debug').insertRow().insertCell().innerHTML = slippage;
    slippage = slippage.toFixed(5);
    $("#"+invMapAlgo[algo_flag]+"_slippage").html(parseInt(slippage));
  } catch(e) {
    alert(e.message); 
  }
}

//改变颜色
function changecolor(id,data) {
  changeStatusColor(id,error_code[data.error_code]);
}

var error_code = {
  '0': "CONFIRM",
  '1': "SEND_ERR",
  '2': "CANCEL_ERR",
  '3': 'BAD_ORDER',
  '4': 'DEAL',
  '5': "CANCELED",
  '6': "PARTIA_CANED",
  '7': "PARTIA_DEAL",
};

//改变颜色
function changeStatusColor(id, error_code) {
  switch (error_code) {
    case "CONFIRM":
      $('#'+id+"_status").attr("class","info");
      break;
    case "SEND_ERR":
      $('#'+id+"_status").attr("class","danger");
      break;
    case "CANCEL_ERR":
      $('#'+id+"_status").attr("class","danger");
      break;
    case 'BAD_ORDER':
      $('#'+id+"_status").attr("class","danger");
      break;
    case 'DEAL':
      $('#'+id+"_status").attr("class","success");
      break;
    case "CANCELED":
      $('#'+id+"_status").attr("class","active");
      break;
    case "PARTIA_CANED":
      $('#'+id+"_status").attr("class","success");
      break;
    case "PARTIA_DEAL":
      $('#'+id+"_status").attr("class","warning");
      break;
  }
}

//在有新order信息到来时，创建新batch
//create a new batch when a order info comes
function newBatch(data) {
  $("#batch-title").after("<tr id='" + data.batch_id + "' onclick = \"onClickBatch(this);\" account='" + data.account + "'> \
                          <td id='" + data.batch_id + "_algo_id'>" + mapBatch[data.batch_id] + "</td> \
                          <td style=\"display:none\" id='" + data.batch_id + "_batch_trade_quantity'>" + "0" + "</td> \
                          <td style=\"display:none\" id='" + data.batch_id + "_batch_quantity'>" + data.trade_quantity + "</td> \
                          <td> \
                             <div class=\"progress\"> \
                                <div class=\"progress-bar\" id='" + data.batch_id + "_batch_progress' role=\"progressbar\" aria-valuenow=\"0\" aria-valuemin=\"0\"  \
                                aria-valuemax=\"100\" style=\"width:0%\"> \
                                    <span style=\"color:black\">0</span>\
                                </div> \
                            </div>\
                          </td> \
                        </tr>"); 
}

//在有新order信息到来时，创建新algo
//create a new algo when a order info comes
function newAlgo(data) {
  $("#algo-title").after(  "<tr id='" + data.alg_order_id + "' data-toggle=\"collapse\" href=\"#" + data.alg_order_id +"_collapse\" onclick = \"onClickAlgo(this);\"  hidden=\"hidden\" account='" + data.account + "'> \
                            <td id='" + data.alg_order_id + "_algo_id'>" + mapAlgo[data.alg_order_id] + "</td> \
                            <td id='" + data.alg_order_id + "_ref_price'>" + " " + "</td> \
                            <td id='" + data.alg_order_id + "_trade_price'>" + " " + "</td> \
                            <td id='" + data.alg_order_id + "_slippage'>" + " " + "</td> \
                            <td style=\"display:none\" id='" + data.alg_order_id + "_algo_trade_quantity'>" + "0" + "</td> \
                            <td style=\"display:none\" id='" + data.alg_order_id + "_algo_quantity'>" + data.trade_quantity + "</td> \
                            <td> \
                               <div class=\"progress\"> \
                                  <div class=\"progress-bar\" id='" + data.alg_order_id + "_algo_progress' role=\"progressbar\" aria-valuenow=\"0\" aria-valuemin=\"0\"  \
                                  aria-valuemax=\"100\" style=\"width:0%\"> \
                                      <span style=\"color:black\">0</span>\
                                  </div> \
                              </div>\
                            </td> \
                          </tr>"); 
}

//在有新order信息到来时，创建新order
//create a new order when a order info comes
function newOrder(data) {
  date = new Date(0);
  s = mapOrder[data.id]
  date.setUTCSeconds(Number(s.substring(0,s.length-6)));
  var sdate = date.format("hh:mm:ss");  



  //document.getElementById('debug').insertRow().insertCell().innerHTML = data.alg_order_id;
  $("#order-title").after(  "<tr class='order' id='" + data.id + "' hidden=\"hidden\" account='" + data.account + "'> \
                            <td id='" + data.id + "_status'>" + "new order" + "</td> \
                            <td id='" + data.id + "_time'>" + sdate + "</td> \
                            <td id='" + data.id + "_code'>" + data.code + "</td> \
                            <td id='" + data.id + "_price'>" + data.limit_price + "</td> \
                            <td style=\"display:none\" id='" + data.id + "_order_trade_quantity'>" + "0" + "</td> \
                            <td style=\"display:none\" id='" + data.id + "_order_quantity'>" + data.trade_quantity + "</td> \
                            <td style=\"display:none\" id='" + data.id + "_batch_flag'>" + data.batch_id + "</td> \
                            <td style=\"display:none\" id='" + data.id + "_algo_flag'>" + data.alg_order_id + "</td> \
                            <td> \
                               <div class=\"progress\"> \
                                  <div class=\"progress-bar\" id='" + data.id + "_order_progress' role=\"progressbar\" aria-valuenow=\"0\" aria-valuemin=\"0\"  \
                                  aria-valuemax=\"100\" style=\"width:0%\"> \
                                      <span style=\"color:black\">0</span>\
                                  </div> \
                              </div>\
                            </td> \
                            <td id='" + data.id + "_open_close'>" + data.open_close + "</td> \
                            <td id='" + data.id + "_accountOrder'>" + data.account + "</td> \
                            <td id='" + data.id + "_buy_sell'>" + data.buy_sell + "</td> \
                            <td id='" + data.id + "_argument_list'>" + data.argument_list + "</td> \
                            <td id='" + data.id + "_id'>" + mapOrder[countOrder.toString()] + "</td> \
                          </tr>"); 
  $('#'+data.id+"_progress").find('span').css("color","black");
  if (data.buy_sell == "SHORT_SELL") {
    $('#'+data.id+'_buy_sell').attr("class","danger");
  } else if (data.buy_sell == "LONG_BUY") {
    $('#'+data.id+'_buy_sell').attr("class","info");
  }
}

//当有新order update到来时，order表主处理函数
//when a new order update comes, the main processing function for order table
function updateOrderTable(data) {
  if (data.cedar_msg_type == "TYPE_ORDER_REQUEST" && data.type != "TYPE_CANCEL_ORDER_REQUEST" ) {

    if (invMapOrder[data.id] != undefined) return;
    
    mapOrder[countOrder.toString()] = data.id;
    invMapOrder[data.id] = countOrder.toString();
    data.id = countOrder.toString();
    
    newOrder(data);
    
    countOrder +=1;
  } else if (data.type != "TYPE_CANCEL_ORDER_REQUEST") {
    data.ref_id = invMapOrder[data.ref_id];
    
    if (data.ref_id == undefined) {
      return;
    }
    updateOrderRow(data.ref_id, data)
  }
}

//当有新order update到来时，algo表的主处理函数
//when a new order update comes, the main processing function for algo table
function updateAlgoTable(data) {
  if (data.cedar_msg_type == "TYPE_ORDER_REQUEST" && data.type != "TYPE_CANCEL_ORDER_REQUEST" ) {

    if (invMapAlgo[data.alg_order_id] != undefined) {
      padAlgoId(data);
      return;
    }
    mapAlgo[countAlgo.toString()] = data.alg_order_id;
    invMapAlgo[data.alg_order_id] = countAlgo.toString();
    data.alg_order_id = countAlgo.toString();
    
    newAlgo(data);
    
    countAlgo +=1;
  } else if (data.type != "TYPE_CANCEL_ORDER_REQUEST") {
    data.ref_id = invMapOrder[data.ref_id];
    
    if (data.ref_id == undefined) {
      return;
    }
    updateAlgoRow(data.alg_order_id, data)
  }   
}

//当有新order update到来时，batch表的主处理函数
//when a new order update comes, the main processing function for batch table
function updateBatchTable(data) {
    if (data.cedar_msg_type == "TYPE_ORDER_REQUEST" && data.type != "TYPE_CANCEL_ORDER_REQUEST" ) {

    if (invMapBatch[data.batch_id] != undefined) return;
    
    mapBatch[countBatch.toString()] = data.batch_id;
    invMapBatch[data.batch_id] = countBatch.toString();
    data.batch_id = countBatch.toString();
    
    newBatch(data);
    
    countBatch +=1;
  } else if (data.type != "TYPE_CANCEL_ORDER_REQUEST") {
    data.ref_id = invMapOrder[data.ref_id];
    
    if (data.ref_id == undefined) {
      return;
    }
    updateBatchRow(data.batch_id, data)
  }
}

function padAlgoId(data) {
  $('#algo tr').each(function() {
    //document.getElementById('debug').insertRow().insertCell().innerHTML = jQuery(this).children(":first").html()+"*****"+data.alg_order_id;
    if (jQuery(this).children(":first").html() == data.alg_order_id) {
      jQuery(this).attr('account',data.account);
    }
  });
} 


function showAllAccount() {
  $("#order tr").each(function() {
    jQuery(this).show();
  });
  $("#algo tr").each(function() {
    jQuery(this).show();
  });
  $("#batch tr").each(function() {
    jQuery(this).show();
  });
}


function onAcctClick(obj) {
  $("#order tr").each(function() {
    if (jQuery(this).attr('id') == "order-title") return;
    //document.getElementById('debug').insertRow().insertCell().innerHTML = jQuery(obj).html()+":"+jQuery(this).attr('account');
    if (jQuery(this).attr('account')!=jQuery(obj).html()) {
      jQuery(this).hide();
    } else {
      jQuery(this).show();
    }
  });
  $("#algo tr").each(function() {
    if (jQuery(this).attr('id') == "algo-title") return;
    //document.getElementById('debug').insertRow().insertCell().innerHTML = jQuery(obj).html()+":"+jQuery(this).attr('account');
    if (jQuery(this).attr('account')!=jQuery(obj).html()) {
      jQuery(this).hide();
    } else {
      jQuery(this).show();
    }
  });
  $("#batch tr").each(function() {
    if (jQuery(this).attr('id') == "batch-title") return;
    //document.getElementById('debug').insertRow().insertCell().innerHTML = jQuery(obj).html()+":"+jQuery(this).attr('account');
    if (jQuery(this).attr('account')!=jQuery(obj).html()) {
      jQuery(this).hide();
    } else {
      jQuery(this).show();
    }
  });
}

function account_check(data) {
  /*if (!data.hasOwnProperty("account")) {
    document.getElementById('debug').insertRow().insertCell().innerHTML = data;
  }*/
  if (data.account==undefined) return;
  if (accountMap[data.account]==undefined) {
    accountMap[data.account] = 1;
    $('#acct_stock').html($('#acct_stock').html() +"<button class='btn acct col-md-1' onclick=\"onAcctClick(this)\">"+ data.account + "</button>");
  }
  
}

var error_code = {
  '0': "CONFIRM",
  '1': "SEND_ERR",
  '2': "CANCEL_ERR",
  '3': 'BAD_ORDER',
  '4': 'DEAL',
  '5': "CANCELED",
  '6': "PARTIA_CANED",
  '7': "PARTIA_DEAL",
};

var countable_states = [
  "CONFIRM",
  'DEAL',
  "PARTIA_CANED",
  "PARTIA_DEAL"
]

var socket_state = [
  "TYPE_ORDER_REQUEST",
  "TYPE_CANCEL_ORDER_CONFIRM",
  "TYPE_TRADE",
  "TYPE_NEW_ORDER_CONFIRM",
  "TYPE_ERROR",
  "TYPE_LIMIT_ORDER_REQUEST"
]

var mapOrder = new Map();
var invMapOrder = new Map();
var mapAlgo = new Map();
var invMapAlgo = new Map();
var mapBatch = new Map();
var invMapBatch = new Map();
var algo_quantity_map = new Map()

var countOrder = 0;
var countAlgo = 0;
var countBatch = 0;
var accountMap = {};

var ws = new WebSocket('ws://192.168.0.66:8213/soc');

//websocket connection
ws.onmessage = function(event) {
  //first level parsing for incoming info
  //首层json反序列化
  //document.getElementById('debug').insertRow().insertCell().innerHTML = event.data;
  try {
    var jData = JSON.parse(event.data);
  } catch (e) {
    alert(e.message);
  }

  //table信息到来时，构建三张表
  //creating rows in batch,algo,order tables, when table info comes from the backend
  if (jData.hasOwnProperty('table')) {
    jData.table = jData.table.replace(/\\/g,"");
    var data = JSON.parse(jData.table);
    var tmp = $.extend(true, {}, data);
    constructOrderTable(tmp);

    tmp = $.extend(true, {}, data);
    constructAlgoTable(tmp);

    tmp = $.extend(true, {}, data);
    constructBatchTable(tmp);
    account_check(data);
  } 

  //当有message类型的信息传入时
  //when info with 'message' type comes in
  else if (jData.hasOwnProperty('message')) {
    jData.message = jData.message.replace(/\\/g,"");
    var data = JSON.parse(jData.message);
    account_check(data);
    if (socket_state.indexOf(data.type) != -1) {          //types not coming from TdxTrade like "SMART_ORDER","APP" should be eliminated
      var tmp = $.extend(true, {}, data);
      updateOrderTable(tmp);

      tmp = $.extend(true, {}, data);
      updateAlgoTable(tmp);

      tmp = $.extend(true, {}, data);
      updateBatchTable(tmp);
    }
    else if (data.type == "TYPE_SMART_ORDER_REQUEST") {
      algo_quantity_map[data.alg_order_id] = data.trade_quantity;
    }
    else {                                                        //dealing with algo info updates
      if (data.type != "APP_STATUS_MSG") return;
      //document.getElementById('debug').insertRow().insertCell().innerHTML = event.data;
      if ($('#'+invMapAlgo[data.alg_order_id]).length>0) {        //if this algo has already been in algo_table
        var data = JSON.parse(jData.message);
        updateAppStatus(data);
        $("#algo").children("*").each(function() {
          if (this.id == 'algo-title') return;
          jQuery(this).children("*").each(function() {
            if (this.id != invMapAlgo[data.alg_order_id]) return;
            computeTradePrice(jQuery(this).children("*").html());
            //document.getElementById('debug').insertRow().insertCell().innerHTML = jQuery(this).children("*").html();
            //document.getElementById('debug').insertRow().insertCell().innerHTML = "*************************";
          });
        });
      } else{                                                       //if this algo is not in algo_table, create this row
        constructAlgoFromAppStatus(data);
      }
    }
  }
  
  //当table信息传输结束时的行为
  //process when table info from backend has finish
  else if (jData.hasOwnProperty('init_finish')) {
    $("#algo").children("*").each(function() {
      if (this.id == 'algo-title') return;
      jQuery(this).children("*").each(function() {
        sumAlgoProgress(jQuery(this).children("*").html());         //compute all algo progress
      });
      
      //sumAlgoProgress(jQuery(this).children("*").children("*").html());
      //computeTradePrice(jQuery(this).children("*").children("*").html())
    });
    $("#batch").children("*").each(function() {
      if (this.id == 'batch-title') return;
      jQuery(this).children("*").each(function() {
        sumBatchProgress(jQuery(this).children("*").html());        ////compute all batch progress
      });
      
      //sumAlgoProgress(jQuery(this).children("*").children("*").html());
    });
  }

}

