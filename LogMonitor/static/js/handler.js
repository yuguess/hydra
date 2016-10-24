//indexedDB.deleteDatabase("cedar")

window.onload = function () {
    $('#order-div').css({
        "position": "fixed",
        "right": "1em",
        "height": "850px"
    });
    $('#algo-div').css({
        "position": "fixed",
        "right": "42%",
        "height": "850px"
    });
    $('#batch-div').css({
        "position": "fixed",
        "left": "1em",
        "height": "850px"
    });
    setInterval("refresh_data();",500);
}

$(document).ready(function() 
    { 
        $("#batch").tablesorter(); 
        $("#algo").tablesorter(); 
        $("#order").tablesorter(); 
        $(".tablesorter").trigger("update"); 
    } 
); 

/*var myDB={
    name:'test',
    version:3,
    db:null
};

openDB("cedar",3)

function openDB (name,version) {
    var version=version || 1;
    var request=window.indexedDB.open(name,version);
    request.onerror=function(e){
        console.log(e.currentTarget.error.message);
    };
    request.onsuccess=function(e){
        myDB.db=e.target.result;
    };
    request.onupgradeneeded=function(e){
        var db=e.target.result;
        if(!db.objectStoreNames.contains('order')){
            db.createObjectStore('order',{keyPath:"id"});
            console.log('order table created');
        }
        if(!db.objectStoreNames.contains('algo')){
            db.createObjectStore('algo',{keyPath:"alg_order_id"});
            console.log('algo table created');
        }
        if(!db.objectStoreNames.contains('batch')){
            db.createObjectStore('batch',{keyPath:"batch_id"});
            console.log('batch table created');
        }
        console.log('DB version changed to '+version);
    };
}


var students=[{ 
            id:1001, 
            name:"Byron", 
            age:24 
        },{ 
            id:1002, 
            name:"Frank", 
            age:30 
        },{ 
            id:1003, 
            name:"Aaron", 
            age:26 
        }];

function addData(db,storeName,data_in){
    var transaction=db.transaction(storeName,'readwrite'); 
    var store=transaction.objectStore(storeName); 
    console.log(data_in)
    obj = { 
            id:1004, 
            name:"Byron", 
            age:24 
        }
    var request = store.add(obj);
    console.log(request);
    for(var i=0;i<students.length;i++){
                store.add(students[i]);
            }
}

function getDataByKey(db,storeName,value,result){
    ready = 0;
    var transaction=db.transaction(storeName,'readwrite'); 
    var store=transaction.objectStore(storeName); 
    var request=store.get(value); 
    request.onsuccess=function(e){ 
        var student=e.target.result; 
        console.log(student); 
        result.push(student)
        ready = 1;
    };

    return request
}

function updateDataByKey(db,storeName,value,data){
            var transaction=db.transaction(storeName,'readwrite'); 
            var store=transaction.objectStore(storeName); 
            var request=store.get(value); 
            request.onsuccess=function(e){ 
                var student=e.target.result; 
                student=data;
                store.put(student); 
            };
}




setTimeout(function(){
    addData(myDB.db,'order',students[0]);
},1000);*/

function showStock() {
    $('#future_div').hide();
    $('#stock_div').show();
}

function showFuture() {
    $('#future_div').show();
    $('#stock_div').hide();
}

//date formating
Date.prototype.format = function (format) {
    var o = {
        "M+": this.getMonth() + 1, //month
        "d+": this.getDate(),    //day
        "h+": this.getHours(),   //hour
        "m+": this.getMinutes(), //minute
        "s+": this.getSeconds(), //second
        "q+": Math.floor((this.getMonth() + 3) / 3),  //quarter
        "S": this.getMilliseconds() //millisecond
    }
    if (/(y+)/.test(format)) format = format.replace(RegExp.$1,
        (this.getFullYear() + "").substr(4 - RegExp.$1.length));
    for (var k in o)if (new RegExp("(" + k + ")").test(format))
        format = format.replace(RegExp.$1,
            RegExp.$1.length == 1 ? o[k] :
                ("00" + o[k]).substr(("" + o[k]).length));
    return format;
}

//更改第二张表algo的reference price
//change the reference price for specific algo row
function updateAppStatus(data) {
    $("#" + invMapAlgo[data.alg_order_id] + "_ref_price").html(Number(data.values).toFixed(4));
    //algo_quantity_db[data.alg_order_id] = data.
    sumBatchProgress(data.batch_id);

    // document.getElementById('debug').insertRow().insertCell().innerHTML = "#"+data.alg_order_id+"_ref_price";
    //document.getElementById('debug').insertRow().insertCell().innerHTML = data.values;
}

//在首次连接时，table信息到来的时候，处理信息
//when connect first time, and table info comes, process 
function constructAlgoTable(data) {
    if (invMapAlgo[data.algo_flag] != undefined) return;
    mapAlgo[countAlgo.toString()] = data.algo_flag;
    invMapAlgo[data.algo_flag] = countAlgo.toString();
    data.algo_flag = countAlgo.toString();

    constructAlgo(data);

    countAlgo += 1;
}



//点击batch的标题栏时，显示所有信息
//show all batch,algo,order info when clicking on the theme of the batch table
function onClickShowAll(data) {
    //$('#audio').play();
    //$("#audio")[0].play();
    $('.tablesorter').trigger('update');

    $("#order tr").each(function () {
        jQuery(this).show();
    });

    $("#algo tr").each(function () {
        jQuery(this).show();
    });
}

//点击batch的标时，显示对应信息
//show proper rows, when clicking on batch row
function onClickBatch(data) {
    var batch_value = jQuery(data).children(":first").html();
   // alert("."+batch_value+"_batch");
    /*$("#order-table-body tr").each(function () {
        jQuery(this).hide();
    });

    $("#algo-table-body tr").each(function () {
        jQuery(this).hide();
    });*/
    $("."+batch_value+"_batch").show()
    $(".title").show()
    current_batch = batch_value;

    active_algo_list = batch_algo_db[batch_value]
    active_order_list = batch_order_db[batch_value]

    $("#algo-table-body tr").each(function () {
        if (!jQuery(this).hasClass("title")) {
            document.getElementById("algo-table-body").deleteRow(this);
        }
        
    });
    $("#order-table-body  tr").each(function () {
        if (jQuery(this).hasClass("title")) {
            return;
        }
        document.getElementById("order-table-body").deleteRow(this);
    });
    click_sig = 1;
}

//点击algo的标时，显示对应信息
//show proper rows, when clicking on algo row
function onClickAlgo(data) {
    /*$("#order-table-body tr").each(function () {
        jQuery(this).hide();
    });*/
    var algo_value = jQuery(data).children(":first").html();
    $("."+algo_value+"_algo").show();
    $(".title").show();

    current_algo = algo_value;

    active_order_list = algo_order_db[algo_value]
    $("#order-table-body  tr").each(function () {
        if (jQuery(this).hasClass("title")) {
            return;
        }
        document.getElementById("order-table-body").deleteRow(this);
    });
    click_sig = 1;
}

//当有新的order update到来时，更改对应order行内容
//update order info when new order update comes
/*function updateOrderRow(id, data) {
    var trade_quantity = trade_quantity_order_db[mapOrder[id]] + Number(data.trade_quantity);
    trade_quantity_order_db[mapOrder[id]] = trade_quantity;
    var tradeString = "" + trade_quantity + "/" + total_quantity_order_db[mapOrder[id]];
    var percentage = 100 * trade_quantity / Number($('#' + id + "_order_quantity").text());

    $('#' + id + "_status").html(error_code[data.error_code]);
    $('#' + id + "_order_trade_quantity").html(trade_quantity);
    $('#' + id + "_order_progress").css({'width': "" + percentage + "%"}).find('span').html(tradeString);

    if (data.error_code == '4') {
        if ($('#' + id + "_buy_sell").html() == "LONG_BUY") {
            $("#audio source").attr("src", "http://tsn.baidu.com/text2audio?tex=" + $('#' + id + "_code").html() + " 已买入" + "\&lan=zh&cuid=" + "94-DE-80-23-E5-A6" + "\&spd=9\&pit=6\&ctp=1&tok=" + "24.e175ed83539ebe33d2eb67c61effe559.2592000.1475223151.282335-8572922");
        } else if ($('#' + id + "_buy_sell").html() == "SHORT_SELL") {
            $("#audio source").attr("src", "http://tsn.baidu.com/text2audio?tex=" + $('#' + id + "_code").html() + " 已卖出" + "\&lan=zh&cuid=" + "94-DE-80-23-E5-A6" + "\&spd=9\&pit=6\&ctp=1&tok=" + "24.e175ed83539ebe33d2eb67c61effe559.2592000.1475223151.282335-8572922");
        }
        $("#audio")[0].pause();
        $("#audio")[0].load();
        $("#audio")[0].play();
    }


    //document.getElementById('debug').insertRow().insertCell().innerHTML = $('#' + id + "_code").html();

    changecolor(id, data);
}*/

//当有新的order update到来时，更改对应batch行内容
//update batch info when new order update comes
function updateBatchRow(batch_id, data) {
    var batch_flag;
    $("td[id$='" + data.ref_id + "_batch_flag']").each(function () {
        batch_flag = this.innerHTML;
    });
    sumBatchProgress(batch_flag);
}

//计算batch进度
//compute batch progress



//当有新的order update到来时，更改对应algo行内容
//update algo info when new order update comes
function updateAlgoRow(algo_id, data) {
    var algo_flag;
    $("td[id$='" + data.ref_id + "_algo_flag']").each(function () {
        algo_flag = this.innerHTML;
    });
    sumAlgoProgress(algo_flag);
    computeTradePrice(algo_flag);
}

//计算algo进度
//compute algo progress

//改变颜色
function changecolor(id, data) {
    changeStatusColor(id, error_code[data.error_code]);
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
            $('#' + id + "_status").attr("class", "info");
            break;
        case "SEND_ERR":
            $('#' + id + "_status").attr("class", "danger");
            break;
        case "CANCEL_ERR":
            $('#' + id + "_status").attr("class", "danger");
            break;
        case 'BAD_ORDER':
            $('#' + id + "_status").attr("class", "danger");
            break;
        case 'DEAL':
            $('#' + id + "_status").attr("class", "success");
            break;
        case "CANCELED":
            $('#' + id + "_status").attr("class", "active");
            break;
        case "PARTIA_CANED":
            $('#' + id + "_status").attr("class", "warning");
            break;
        case "PARTIA_DEAL":
            $('#' + id + "_status").attr("class", "warning");
            break;
    }
}

//在有新order信息到来时，创建新batch
//create a new batch when a order info comes
function newBatch(data) {
    $("#batch-title").after("<tr class = '" + data.account + "' id='" + data.batch_id + "' onclick = \"onClickBatch(this);\" account='" + data.account + "'> \
                          <td id='" + data.batch_id + "_algo_id'>" + mapBatch[data.batch_id] + "</td> \
                          <td style=\"display:none\" id='" + data.batch_id + "_batch_trade_quantity'>" + "0" + "</td> \
                          <td style=\"display:none\" id='" + data.batch_id + "_batch_quantity'>" + data.trade_quantity + "</td> \
                          <td id='" + data.batch_id + "_batch_notional'>" + "" + "</td> \
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

function showAllAccount() {
    $("#order tr").each(function () {
        jQuery(this).show();
    });
    $("#algo tr").each(function () {
        jQuery(this).show();
    });
    $("#batch tr").each(function () {
        jQuery(this).show();
    });
}

function onAcctClick(obj) {
    $("#order-table-body tr").each(function () {
        if (jQuery(this).attr('id') == "order-title") return;
        //document.getElementById('debug').insertRow().insertCell().innerHTML = jQuery(obj).html()+":"+jQuery(this).attr('account');
        if (jQuery(this).attr('account') != jQuery(obj).html()) {
            jQuery(this).hide();
        } else {
            jQuery(this).show();
        }
    });
    $("#algo-table-body tr").each(function () {
        if (jQuery(this).attr('id') == "algo-title") return;
        //document.getElementById('debug').insertRow().insertCell().innerHTML = jQuery(obj).html()+":"+jQuery(this).attr('account');
        if (!jQuery(this).hasClass(jQuery(obj).html())) {
            jQuery(this).hide();
        } else {
            jQuery(this).show();
        }
    });
    $("#batch-table-body tr").each(function () {
        if (jQuery(this).attr('id') == "batch-title") return;
        //document.getElementById('debug').insertRow().insertCell().innerHTML = jQuery(obj).html()+":"+jQuery(this).attr('account');
        if (!jQuery(this).hasClass(jQuery(obj).html())) {
            jQuery(this).hide();
        } else {
            jQuery(this).show();
        }
    });
    current_account = jQuery(obj).html();
}

function account_check(data) {
    /*if (!data.hasOwnProperty("account")) {
     document.getElementById('debug').insertRow().insertCell().innerHTML = data;
     }*/
    if (data.account == undefined) return;
    if (accountMap[data.account] == undefined) {
        accountMap[data.account] = 1;
        $('#acct_stock').html($('#acct_stock').html() + "<button class='btn acct col-md-1' onclick=\"onAcctClick(this)\">" + data.account + "</button>");
    }
}

function constructOrder(data) {
    if (data.account!=undefined && accountMap[data.account]==undefined) {
        $('#acct_stock').html($('#acct_stock').html() + "<button class='btn acct col-md-1' onclick=\"onAcctClick(this)\">" + data.account + "</button>");
        accountMap[data.account] = 1;
    }

    order_data_db[data.id] = data;

    if (batch_order_db[data.batch_id] == undefined) {
        batch_order_db[data.batch_id] = [data.id];
    } else {
        batch_order_db[data.batch_id].push(data.id);
    }

    if (algo_order_db[data.alg_order_id] == undefined) {
        algo_order_db[data.alg_order_id] = [data.id];
    } else {
        algo_order_db[data.alg_order_id].push(data.id);
    }

    /*date = new Date(0);
    s = data.id;
    date.setUTCSeconds(Number(s.substring(0, 10)));
    var sdate = date.format("hh:mm:ss");

    var percentage = Number(data.trade_quantity) / Number(data.quantity);
    $("#order-table-body").prepend("<tr style=\"display:none\" class='order " + data.alg_order_id +"_algo "+data.batch_id+"_batch' id='" + data.id + "' account='" + data.account+"'> \
                            <td id='" + data.id + "_status'>" + data.status + "</td> \
                            <td id='" + data.id + "_time'>" + sdate + "</td> \
                            <td id='" + data.id + "_code'>" + data.code + "</td> \
                            <td id='" + data.id + "_price'>" + Number(data.price).toFixed(4) + "</td> \
                            <td style=\"display:none\" id='" + data.id + "_order_trade_quantity'>" + data.trade_quantity + "</td> \
                            <td style=\"display:none\" id='" + data.id + "_order_quantity'>" + data.quantity + "</td> \
                            <td style=\"display:none\" id='" + data.id + "_batch_flag'>" + data.batch_id + "</td> \
                            <td style=\"display:none\" id='" + data.id + "_algo_flag'>" + data.alg_order_id + "</td> \
                            <td> \
                               <div class=\"progress\"> \
                                  <div class=\"progress-bar\" id='" + data.id + "_order_progress' role=\"progressbar\" aria-valuenow=\"0\" aria-valuemin=\"0\"  \
                                  aria-valuemax=\"100\" style=\"width:" + 100 * percentage + "%\"> \
                                      <span style=\"color:black\">" + data.trade_quantity + '/' + data.quantity + "</span>\
                                  </div> \
                              </div>\
                            </td> \
                            <td id='" + data.id + "_open_close'>" + data.open_close + "</td> \
                            <td id='" + data.id + "_accountOrder'>" + data.account + "</td> \
                            <td id='" + data.id + "_buy_sell'>" + data.buy_sell + "</td> \
                            <td id='" + data.id + "_argument_list'>" + data.argument_list + "</td> \
                            <td id='" + data.id + "_id'>" + data.id + "</td> \
                          </tr>");
    $('#' + data.id + "_progress").find('span').css("color", "black");
    if (data.buy_sell == "SHORT_SELL") {
        $('#' + data.id + '_buy_sell').attr("class", "danger");
    } else if (data.buy_sell == "LONG_BUY") {
        $('#' + data.id + '_buy_sell').attr("class", "info");
    }
    changeStatusColor(data.id, data.status)
    if (current_account!=undefined && current_account!= data.account) {
        $('#' + data.id).hide();
    }
    if (current_algo!=undefined && current_algo!= data.alg_order_id) {
        $('#' + data.id).hide();
    }
    if (current_batch!=undefined && current_batch!= data.batch_id) {
        $('#' + data.id).hide();
    }

    if (data.trade_quantity>=data.quantity) {
    if ($('#' + data.id + "_buy_sell").html() == "LONG_BUY") {
      $("#audio source").attr("src","http://tsn.baidu.com/text2audio?tex=" + $('#' + data.id + "_code").html() + " 已买入" + "\&lan=zh&cuid=" + "94-DE-80-23-E5-A6" + "\&spd=9\&pit=6\&ctp=1&tok="+"24.90689ceccd1622d4679f82e1e630f7ce.2592000.1478668346.282335-8572922");
    } else if ($('#' + data.id + "_buy_sell").html() == "SHORT_SELL") {
      $("#audio source").attr("src","http://tsn.baidu.com/text2audio?tex=" + $('#' + data.id + "_code").html() + " 已卖出" + "\&lan=zh&cuid=" + "94-DE-80-23-E5-A6" + "\&spd=9\&pit=6\&ctp=1&tok="+"24.90689ceccd1622d4679f82e1e630f7ce.2592000.1478668346.282335-8572922");
    }
    $("#audio")[0].pause();
    $("#audio")[0].load();
    $("#audio")[0].play();
  }*/
}

//在首次连接时，table信息到来的时候，创建algo行
//when connect first time, and table info comes, create a algo row
function constructAlgo(data) {
    if (data.account!=undefined && accountMap[data.account]==undefined) {
        $('#acct_stock').html($('#acct_stock').html() + "<button class='btn acct col-md-1' onclick=\"onAcctClick(this)\">" + data.account + "</button>");
        accountMap[data.account] = 1;
    }
    algo_data_db[data.alg_order_id] = data;

    if (batch_algo_db[data.batch_id] == undefined) {
        batch_algo_db[data.batch_id] = [data.alg_order_id];
    } else {
        batch_algo_db[data.batch_id].push(data.alg_order_id);
    }


    /*var percentage = Number(data.trade_quantity) / Number(data.quantity);

    if (data.ref_price!='') {
        data.ref_price = Number(data.ref_price).toFixed(3)
    }
    if (data.trade_price!='') {
        data.trade_price = Number(data.trade_price).toFixed(3)
    }
    if (data.slippage!='') {
        data.slippage = Number(data.slippage).toFixed(3)
    }
    if (data.notional=='') {
        data.notional = {"buy":0, "sell":0}
    }

    $("#algo-table-body").prepend("<tr style=\"display:none\" class = '" + data.batch_id + "_batch' id='" + data.alg_order_id + "' data-toggle=\"collapse\" href=\"#" + data.alg_order_id + "_collapse\" onclick = \"onClickAlgo(this);\" account='" + data.account + "'> \
                            <td style=\"display:none\" id='" + data.alg_order_id + "'>" + data.alg_order_id + "</td> \
                            <td id='" + data.alg_order_id + "_code'>" + data.code + "</td> \
                            <td width=15% id='" + data.alg_order_id + "_name'>" + stockMap[data.code] + "</td> \
                            <td id='" + data.alg_order_id + "_ref_price'>" + data.ref_price + "</td> \
                            <td id='" + data.alg_order_id + "_trade_price'>" + Number(data.trade_price) + "</td> \
                            <td id='" + data.alg_order_id + "_slippage'>" + Number(data.slippage) + "</td> \
                            <td style=\"display:none\" id='" + data.alg_order_id + "_algo_trade_quantity'>" + "0" + "</td> \
                            <td style=\"display:none\" id='" + data.alg_order_id + "_algo_quantity'>" + data.trade_quantity + "</td> \
                            <td id='" + data.alg_order_id + "_algo_notional'>" + data.notional.buy.toFixed(1)+"/"+data.notional.sell.toFixed(1) + "</td> \
                            <td width=20%> \
                              <div class=\"progress\"> \
                                  <div class=\"progress-bar\" id='" + data.alg_order_id + "_algo_progress' role=\"progressbar\" aria-valuenow=\"0\" aria-valuemin=\"0\"  \
                                  aria-valuemax=\"100\" style=\"width:" + 100 * percentage + "%\"> \
                                      <span style=\"color:black\">" + data.trade_quantity + '/' + data.quantity + "</span>\
                                  </div> \
                              </div>\
                            </td> \
                          </tr>");
    if (current_account!=undefined && current_account!= data.account) {
        $('#' + data.alg_order_id).hide();
    }
    if (current_batch!=undefined && current_batch!= data.batch_id) {
        $('#' + data.alg_order_id).hide();
    }*/
}

function constructBatch(data) {
    if (data.account!=undefined && accountMap[data.account]==undefined) {
        $('#acct_stock').html($('#acct_stock').html() + "<button class='btn acct col-md-1' onclick=\"onAcctClick(this)\">" + data.account + "</button>");
        accountMap[data.account] = 1;
    }

    batch_data_db[data.batch_id] = data;

    if (data.notional=='') {
        data.notional = {"buy":0, "sell":0}
    }
    var percentage = Number(data.trade_quantity) / Number(data.quantity);
    $("#batch-table-body").prepend("<tr class = '" + data.account + "' id='" + data.batch_id + "' onclick = \"onClickBatch(this);\" account='" + data.account + "'> \
                          <td id='" + data.batch_id + "'>" + data.batch_id + "</td> \
                          <td style=\"display:none\" id='" + data.batch_id + "_batch_trade_quantity'>" + data.trade_quantity  + "</td> \
                          <td style=\"display:none\" id='" + data.batch_id + "_batch_quantity'>" + data.quantity + "</td> \
                          <td id='" + data.batch_id + "_batch_notional'>" + data.notional.buy.toFixed(1)+"/"+data.notional.sell.toFixed(1) + "</td> \
                          <td> \
                             <div class=\"progress\"> \
                                <div class=\"progress-bar\" id='" + data.batch_id + "_batch_progress' role=\"progressbar\" aria-valuenow=\"0\" aria-valuemin=\"0\"  \
                                aria-valuemax=\"100\" style=\"width:" + 100 * percentage + "%\"> \
                                    <span style=\"color:black\">" + (100 * percentage).toFixed(2) + '%' + "</span>\
                                </div> \
                            </div>\
                          </td> \
                        </tr>");
    if (current_account!=undefined && current_account!= data.account) {
        $('#' + data.batch_id).hide();
    }
}

function changeOrder(data) {

    debug_count_order += 1;
    
    order_data_db[data.id] = data;

    if (!(data.id in refresh_order_list)) {
        refresh_order_list[data.id] = 1;
    } 

    /*orderObj = mapOrder[obj.id];
    var data = obj;

    date = new Date(0);
    s = data.id;
    date.setUTCSeconds(Number(s.substring(0, 10)));
    var sdate = date.format("hh:mm:ss");

    var percentage = Number(data.trade_quantity) / Number(data.quantity);
    $("#"+data.id).replaceWith("<tr class='order " + data.alg_order_id +"_algo "+data.batch_id+"_batch' id='" + data.id + "' account='" + data.account+"'> \
                            <td id='" + data.id + "_status'>" + data.status + "</td> \
                            <td id='" + data.id + "_time'>" + sdate + "</td> \
                            <td id='" + data.id + "_code'>" + data.code + "</td> \
                            <td id='" + data.id + "_price'>" + Number(data.price).toFixed(4) + "</td> \
                            <td style=\"display:none\" id='" + data.id + "_order_trade_quantity'>" + data.trade_quantity + "</td> \
                            <td style=\"display:none\" id='" + data.id + "_order_quantity'>" + data.quantity + "</td> \
                            <td style=\"display:none\" id='" + data.id + "_batch_flag'>" + data.batch_id + "</td> \
                            <td style=\"display:none\" id='" + data.id + "_algo_flag'>" + data.alg_order_id + "</td> \
                            <td> \
                               <div class=\"progress\"> \
                                  <div class=\"progress-bar\" id='" + data.id + "_order_progress' role=\"progressbar\" aria-valuenow=\"0\" aria-valuemin=\"0\"  \
                                  aria-valuemax=\"100\" style=\"width:" + 100 * percentage + "%\"> \
                                      <span style=\"color:black\">" + data.trade_quantity + '/' + data.quantity + "</span>\
                                  </div> \
                              </div>\
                            </td> \
                            <td id='" + data.id + "_open_close'>" + data.open_close + "</td> \
                            <td id='" + data.id + "_accountOrder'>" + data.account + "</td> \
                            <td id='" + data.id + "_buy_sell'>" + data.buy_sell + "</td> \
                            <td id='" + data.id + "_argument_list'>" + data.argument_list + "</td> \
                            <td id='" + data.id + "_id'>" + data.id + "</td> \
                          </tr>");
    $('#' + data.id + "_progress").find('span').css("color", "black");
    if (data.buy_sell == "SHORT_SELL") {
        $('#' + data.id + '_buy_sell').attr("class", "danger");
    } else if (data.buy_sell == "LONG_BUY") {
        $('#' + data.id + '_buy_sell').attr("class", "info");
    }
    changeStatusColor(data.id, data.status)
    //document.getElementById('debug').insertRow().insertCell().innerHTML = orderObj;
  if (data.trade_quantity>=data.quantity) {
    if ($('#' + data.id + "_buy_sell").html() == "LONG_BUY") {
      $("#audio source").attr("src","http://tsn.baidu.com/text2audio?tex=" + $('#' + data.id + "_code").html() + " 已买入" + "\&lan=zh&cuid=" + "94-DE-80-23-E5-A6" + "\&spd=9\&pit=6\&ctp=1&tok="+"24.90689ceccd1622d4679f82e1e630f7ce.2592000.1478668346.282335-8572922");
    } else if ($('#' + data.id + "_buy_sell").html() == "SHORT_SELL") {
      $("#audio source").attr("src","http://tsn.baidu.com/text2audio?tex=" + $('#' + data.id + "_code").html() + " 已卖出" + "\&lan=zh&cuid=" + "94-DE-80-23-E5-A6" + "\&spd=9\&pit=6\&ctp=1&tok="+"24.90689ceccd1622d4679f82e1e630f7ce.2592000.1478668346.282335-8572922");
    }
    $("#audio")[0].pause();
    $("#audio")[0].load();
    $("#audio")[0].play();
  }

    if (current_account!=undefined && current_account!= data.account) {
        $('#' + data.id).hide();
    }
    if (current_algo!=undefined && current_algo!= data.alg_order_id) {
        $('#' + data.id).hide();
    }
    if (current_batch!=undefined && current_batch!= data.batch_id) {
        $('#' + data.id).hide();
    }*/
}




function changeAlgo(data) {
    debug_count_algo += 1;
    algo_data_db[data.alg_order_id] = data;

    if (!(data.alg_order_id in refresh_algo_list)) {
        refresh_algo_list[data.alg_order_id] = 1;
    } 
    /*data = obj;
    var percentage = Number(data.trade_quantity) / Number(data.quantity);

    if (data.ref_price!='') {
        data.ref_price = Number(data.ref_price).toFixed(3)
    }
    if (data.trade_price!='') {
        data.trade_price = Number(data.trade_price).toFixed(3)
    }
    if (data.slippage!='') {
        data.slippage = Number(data.slippage).toFixed(3)
    }
    if (data.notional=='') {
        data.notional = {"buy":0, "sell":0}
    }

    $('#' + data.alg_order_id + "_ref_price")

    $("#"+data.alg_order_id).replaceWith("<tr class = '" + data.batch_id + "_batch' id='" + data.alg_order_id + "' data-toggle=\"collapse\" href=\"#" + data.alg_order_id + "_collapse\" onclick = \"onClickAlgo(this);\" account='" + data.account + "'> \
                            <td style=\"display:none\" id='" + data.alg_order_id + "'>" + data.alg_order_id + "</td> \
                            <td id='" + data.alg_order_id + "_code'>" + data.code + "</td> \
                            <td width=15% id='" + data.alg_order_id + "_name'>" + "stockMap[data.code]" + "</td> \
                            <td id='" + data.alg_order_id + "_ref_price'>" + data.ref_price + "</td> \
                            <td id='" + data.alg_order_id + "_trade_price'>" + "Number(data.trade_price)" + "</td> \
                            <td id='" + data.alg_order_id + "_slippage'>" + "Number(data.slippage)" + "</td> \
                            <td style=\"display:none\" id='" + data.alg_order_id + "_algo_trade_quantity'>" + "0" + "</td> \
                            <td style=\"display:none\" id='" + data.alg_order_id + "_algo_quantity'>" + data.trade_quantity + "</td> \
                            <td id='" + data.alg_order_id + "_algo_notional'>" + data.notional.buy.toFixed(1)+"/"+data.notional.sell.toFixed(1) + "</td> \
                            <td width=20%> \
                              <div class=\"progress\"> \
                                  <div class=\"progress-bar\" id='" + data.alg_order_id + "_algo_progress' role=\"progressbar\" aria-valuenow=\"0\" aria-valuemin=\"0\"  \
                                  aria-valuemax=\"100\" style=\"width:" + 100 * percentage + "%\"> \
                                      <span style=\"color:black\">" + data.trade_quantity + '/' + data.quantity + "</span>\
                                  </div> \
                              </div>\
                            </td> \
                          </tr>");*/
    if (current_account!=undefined && current_account!= data.account) {
        $('#' + data.alg_order_id).hide();
    }
    if (current_batch!=undefined && current_batch!= data.batch_id) {
        $('#' + data.alg_order_id).hide();
    }
}



function changeBatch(data) {
    batch_data_db[data.batch_id] = data;

    if (!(data.batch_id in refresh_batch_list)) {
        refresh_batch_list[data.batch_id] = 1;
    } 
    /*debug_count_batch += 1;
    data= obj
    var percentage = Number(data.trade_quantity) / Number(data.quantity);
    if (data.notional=='') {
        data.notional = {"buy":0, "sell":0}
    }
    $("#"+data.batch_id).replaceWith("<tr class = '" + data.account + "' id='" + data.batch_id + "' onclick = \"onClickBatch(this);\" account='" + data.account + "'> \
                          <td id='" + data.batch_id + "'>" + data.batch_id + "</td> \
                          <td style=\"display:none\" id='" + data.batch_id + "_batch_trade_quantity'>" + data.trade_quantity + "</td> \
                          <td style=\"display:none\" id='" + data.batch_id + "_batch_quantity'>" + data.quantity + "</td> \
                          <td id='" + data.batch_id + "_batch_notional'>" + data.notional.buy.toFixed(1)+"/"+data.notional.sell.toFixed(1) + "</td> \
                          <td> \
                             <div class=\"progress\"> \
                                <div class=\"progress-bar\" id='" + data.batch_id + "_batch_progress' role=\"progressbar\" aria-valuenow=\"0\" aria-valuemin=\"0\"  \
                                aria-valuemax=\"100\" style=\"width:" + 100 * percentage + "%\"> \
                                    <span style=\"color:black\">" + (100 * percentage).toFixed(2) + '%'  + "</span>\
                                </div> \
                            </div>\
                          </td> \
                        </tr>");
    $('#' + data.batch_id + "_batch_progress").css({'width': "" + percentage + "%"});


    if (current_account!=undefined && current_account!= data.account) {
        $('#' + data.batch_id).hide();
    }*/
}

function updateOrderTable(id, data) {
    data = data.replace(/\'/g, "\"");
    var obj = JSON.parse(data); 
    //document.getElementById('debug').insertRow().insertCell().innerHTML = data;
    if (mapOrder[id]==undefined) {
        constructOrder(obj);
        mapOrder[id] = document.getElementById(obj.id);
    }
    else {
        changeOrder(obj)
    }
}

function updateAlgoTable(id, data) {
    data = data.replace(/\'/g, "\"");
    var obj = JSON.parse(data); 
    //document.getElementById('debug').insertRow().insertCell().innerHTML = data;
    if (mapAlgo[id]==undefined) {
        constructAlgo(obj);
        mapAlgo[id] = document.getElementById(obj.alg_order_id);
    } 
    else {
       changeAlgo(obj);
    }
}

function updateBatchTable(id, data) {
    //document.getElementById('debug').insertRow().insertCell().innerHTML = ""+id+data;
    data = data.replace(/\'/g, "\"");
    var obj = JSON.parse(data); 
    //document.getElementById('debug').insertRow().insertCell().innerHTML = data;
    if (mapBatch[id]==undefined) {
        constructBatch(obj);
        mapBatch[id] = document.getElementById(obj.batch_id);
    } 
    else {
        changeBatch(obj);
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
    '7': "PARTIA_DEAL"
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


var debug_count_algo = 0;
var debug_count_order = 0;
var debug_count_batch = 0;

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

var batch_data_db = {}
var algo_data_db = {}
var order_data_db = {}

//reltion db
var batch_order_db = {};
var algo_order_db = {};
var batch_algo_db = {};

var active_algo_list = {};
var active_order_list = {};

var refresh_batch_list = {};
var refresh_algo_list = {};
var refresh_order_list = {};

var current_algo = undefined;
var current_order = undefined;
var current_batch = undefined;
var current_user = undefined;
var current_account = undefined;

var click_sig_batch = 0;
var click_sig_algo = 0;

var ws1 = new WebSocket('ws://192.168.0.66:8082/soc');

//websocket connection
ws1.onmessage = function (event) {
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
    if (!jData.hasOwnProperty('type')) {
        return
    }
    //jData.value = jData.value.replace(/\\/g, "");
    //var data = JSON.parse(jData.value);
    if (jData.type == "order") {
        updateOrderTable(jData.key,jData.value);
    }
    else if (jData.type == "algo") {
        updateAlgoTable(jData.key,jData.value);
    }
    else if (jData.type == "batch") {
        updateBatchTable(jData.key,jData.value);
    }
    return
}

var wss = new WebSocket('ws://192.168.0.66:8001/soc');

//websocket connection
wss.onmessage = function (event) {
    try {
        var jData = JSON.parse(event.data);
    } catch (e) {
        alert(e.message);
    }
//document.getElementById('debug').insertRow().insertCell().innerHTML = event.data;
    //table信息到来时，构建三张表
    //creating rows in batch,algo,order tables, when table info comes from the backend
    if (jData.hasOwnProperty('process_status')) {
        jData.process_status = jData.process_status.replace(/\\/g, "");
        var data = JSON.parse(jData.process_status);
        //document.getElementById('debug').insertRow().insertCell().innerHTML = event.data;
        for (var key in data) {
            if (data[key] == true) {
                var key_tmp = key;
                if (key_tmp == 'MonitorServer.p') key_tmp = 'MonitorServer';
                $('#' + key_tmp).attr('class', 'btn btn-success').html('o');
                //document.getElementById('debug').insertRow().insertCell().innerHTML = key;
            } else if (data[key] == false) {
                var key_tmp = key;
                if (key_tmp == 'MonitorServer.p') key_tmp = 'MonitorServer';
                $('#' + key_tmp).attr('class', 'btn btn-danger').html('x');
            }
        }
    }
}

function refresh_data() {
    dup_algo_data_db = jQuery.extend(true,{},algo_data_db);
    dup_order_data_db = jQuery.extend(true,{},order_data_db);

    for (item in batch_data_db) {
        refresh_batch_data(batch_data_db[item]);
    }
    for (item in active_algo_list)  {
        //document.getElementById('debug').insertRow().insertCell().innerHTML = active_algo_list[item];
        refresh_algo_data(active_algo_list[item]);
    }
    for (item in active_order_list) {
        //document.getElementById('debug').insertRow().insertCell().innerHTML = active_order_list[item];
        refresh_order_data(active_order_list[item]);
    }
    refresh_order_list = [];
    refresh_algo_list = [];
    refresh_batch_list = [];

    $(".tablesorter").trigger("update"); 
}

function refresh_batch_data(data) {
    debug_count_batch += 1;

    var percentage = Number(data.trade_quantity) / Number(data.quantity);
    if (data.notional=='') {
        data.notional = {"buy":0, "sell":0}
    }
    $("#"+data.batch_id).replaceWith("<tr class = '" + data.account + "' id='" + data.batch_id + "' onclick = \"onClickBatch(this);\" account='" + data.account + "'> \
                          <td id='" + data.batch_id + "'>" + data.batch_id + "</td> \
                          <td style=\"display:none\" id='" + data.batch_id + "_batch_trade_quantity'>" + data.trade_quantity + "</td> \
                          <td style=\"display:none\" id='" + data.batch_id + "_batch_quantity'>" + data.quantity + "</td> \
                          <td id='" + data.batch_id + "_batch_notional'>" + data.notional.buy.toFixed(1)+"/"+data.notional.sell.toFixed(1) + "</td> \
                          <td> \
                             <div class=\"progress\"> \
                                <div class=\"progress-bar\" id='" + data.batch_id + "_batch_progress' role=\"progressbar\" aria-valuenow=\"0\" aria-valuemin=\"0\"  \
                                aria-valuemax=\"100\" style=\"width:" + 100 * percentage + "%\"> \
                                    <span style=\"color:black\">" + (100 * percentage).toFixed(2) + '%'  + "</span>\
                                </div> \
                            </div>\
                          </td> \
                        </tr>");
    $('#' + data.batch_id + "_batch_progress").css({'width': "" + 100*percentage + "%"});
    if (current_account!=undefined && current_account!= data.account) {
        $('#' + data.batch_id).hide();
    }
}

function refresh_algo_data(item) {
    
    data = jQuery.extend(true,{},dup_algo_data_db[item]);
    var percentage = Number(data.trade_quantity) / Number(data.quantity);

    if (data.ref_price!='') {
        data.ref_price = Number(data.ref_price).toFixed(3)
    }
    if (data.trade_price!='') {
        data.trade_price = Number(data.trade_price).toFixed(3)
    }
    if (data.slippage!='') {
        data.slippage = Number(data.slippage).toFixed(3)
    }
    if (data.notional=='') {
        data.notional = {"buy":0, "sell":0}
    }
    if ($("#"+data.alg_order_id).length == 0 && data.alg_order_id!="") {
        $("#algo-table-body").prepend("<tr class = '" + data.batch_id + "_batch' id='" + data.alg_order_id + "' data-toggle=\"collapse\" href=\"#" + data.alg_order_id + "_collapse\" onclick = \"onClickAlgo(this);\" account='" + data.account + "'> \
                <td style=\"display:none\" id='" + data.alg_order_id + "'>" + data.alg_order_id + "</td> \
                <td id='" + data.alg_order_id + "_code'>" + data.code + "</td> \
                <td width=15% id='" + data.alg_order_id + "_name'>" + stockMap[data.code] + "</td> \
                <td id='" + data.alg_order_id + "_ref_price'>" + data.ref_price + "</td> \
                <td id='" + data.alg_order_id + "_trade_price'>" + Number(data.trade_price) + "</td> \
                <td id='" + data.alg_order_id + "_slippage'>" + Number(data.slippage) + "</td> \
                <td style=\"display:none\" id='" + data.alg_order_id + "_algo_trade_quantity'>" + "0" + "</td> \
                <td style=\"display:none\" id='" + data.alg_order_id + "_algo_quantity'>" + data.trade_quantity + "</td> \
                <td id='" + data.alg_order_id + "_algo_notional'>" + data.notional.buy.toFixed(1)+"/"+data.notional.sell.toFixed(1) + "</td> \
                <td width=20%> \
                  <div class=\"progress\"> \
                      <div class=\"progress-bar\" id='" + data.alg_order_id + "_algo_progress' role=\"progressbar\" aria-valuenow=\"0\" aria-valuemin=\"0\"  \
                      aria-valuemax=\"100\" style=\"width:" + 100 * percentage + "%\"> \
                          <span style=\"color:black\">" + data.trade_quantity + '/' + data.quantity + "</span>\
                      </div> \
                  </div>\
                </td> \
              </tr>");
    } else {
        if (!(item in refresh_algo_list)  && click_sig_batch == 0) {
            return;
        }
        click_sig_batch = 0;
        $("#"+data.alg_order_id).replaceWith("<tr class = '" + data.batch_id + "_batch' id='" + data.alg_order_id + "' data-toggle=\"collapse\" href=\"#" + data.alg_order_id + "_collapse\" onclick = \"onClickAlgo(this);\" account='" + data.account + "'> \
                        <td style=\"display:none\" id='" + data.alg_order_id + "'>" + data.alg_order_id + "</td> \
                        <td id='" + data.alg_order_id + "_code'>" + data.code + "</td> \
                        <td width=15% id='" + data.alg_order_id + "_name'>" + stockMap[data.code] + "</td> \
                        <td id='" + data.alg_order_id + "_ref_price'>" + data.ref_price + "</td> \
                        <td id='" + data.alg_order_id + "_trade_price'>" + Number(data.trade_price) + "</td> \
                        <td id='" + data.alg_order_id + "_slippage'>" + Number(data.slippage) + "</td> \
                        <td style=\"display:none\" id='" + data.alg_order_id + "_algo_trade_quantity'>" + "0" + "</td> \
                        <td style=\"display:none\" id='" + data.alg_order_id + "_algo_quantity'>" + data.trade_quantity + "</td> \
                        <td id='" + data.alg_order_id + "_algo_notional'>" + data.notional.buy.toFixed(1)+"/"+data.notional.sell.toFixed(1) + "</td> \
                        <td width=20%> \
                          <div class=\"progress\"> \
                              <div class=\"progress-bar\" id='" + data.alg_order_id + "_algo_progress' role=\"progressbar\" aria-valuenow=\"0\" aria-valuemin=\"0\"  \
                              aria-valuemax=\"100\" style=\"width:" + 100 * percentage + "%\"> \
                                  <span style=\"color:black\">" + data.trade_quantity + '/' + data.quantity + "</span>\
                              </div> \
                          </div>\
                        </td> \
                      </tr>");
    }

}

function refresh_order_data(item) {

    //orderObj = mapOrder[obj.id];
    var data = jQuery.extend(true,{},dup_order_data_db[item]);
    if (data==undefined) {
        return;
    }
    date = new Date(0);
    s = data.id;
    date.setUTCSeconds(Number(s.substring(0, 10)));
    //var sdate = date.format("hh:mm:ss");
    sdate = 0;

    var percentage = Number(data.trade_quantity) / Number(data.quantity);

    if ($("#"+data.id).length == 0) {
        $("#order").prepend("<tr class='order " + data.alg_order_id +"_algo "+data.batch_id+"_batch' id='" + data.id + "' account='" + data.account+"'> \
                                        <td id='" + data.id + "_status'>" + data.status + "</td> \
                                        <td id='" + data.id + "_time'>" + sdate + "</td> \
                                        <td id='" + data.id + "_code'>" + data.code + "</td> \
                                        <td id='" + data.id + "_price'>" + Number(data.price).toFixed(4) + "</td> \
                                        <td style=\"display:none\" id='" + data.id + "_order_trade_quantity'>" + data.trade_quantity + "</td> \
                                        <td style=\"display:none\" id='" + data.id + "_order_quantity'>" + data.quantity + "</td> \
                                        <td style=\"display:none\" id='" + data.id + "_batch_flag'>" + data.batch_id + "</td> \
                                        <td style=\"display:none\" id='" + data.id + "_algo_flag'>" + data.alg_order_id + "</td> \
                                        <td> \
                                           <div class=\"progress\"> \
                                              <div class=\"progress-bar\" id='" + data.id + "_order_progress' role=\"progressbar\" aria-valuenow=\"0\" aria-valuemin=\"0\"  \
                                              aria-valuemax=\"100\" style=\"width:" + 100 * percentage + "%\"> \
                                                  <span style=\"color:black\">" + data.trade_quantity + '/' + data.quantity + "</span>\
                                              </div> \
                                          </div>\
                                        </td> \
                                        <td id='" + data.id + "_open_close'>" + data.open_close + "</td> \
                                        <td id='" + data.id + "_accountOrder'>" + data.account + "</td> \
                                        <td id='" + data.id + "_buy_sell'>" + data.buy_sell + "</td> \
                                        <td id='" + data.id + "_argument_list'>" + data.argument_list + "</td> \
                                        <td id='" + data.id + "_id'>" + data.id + "</td> \
                                    </tr>");
    }
    else {
        if (!(item in refresh_order_list) && click_sig_algo == 0) {
           // document.getElementById('debug').insertRow().insertCell().innerHTML = "aaaa";
            return;
        }
        click_sig_algo = 0;
        $("#"+data.id).replaceWith("<tr class='order " + data.alg_order_id +"_algo "+data.batch_id+"_batch' id='" + data.id + "' account='" + data.account+"'> \
                                        <td id='" + data.id + "_status'>" + data.status + "</td> \
                                        <td id='" + data.id + "_time'>" + sdate + "</td> \
                                        <td id='" + data.id + "_code'>" + data.code + "</td> \
                                        <td id='" + data.id + "_price'>" + Number(data.price).toFixed(4) + "</td> \
                                        <td style=\"display:none\" id='" + data.id + "_order_trade_quantity'>" + data.trade_quantity + "</td> \
                                        <td style=\"display:none\" id='" + data.id + "_order_quantity'>" + data.quantity + "</td> \
                                        <td style=\"display:none\" id='" + data.id + "_batch_flag'>" + data.batch_id + "</td> \
                                        <td style=\"display:none\" id='" + data.id + "_algo_flag'>" + data.alg_order_id + "</td> \
                                        <td> \
                                           <div class=\"progress\"> \
                                              <div class=\"progress-bar\" id='" + data.id + "_order_progress' role=\"progressbar\" aria-valuenow=\"0\" aria-valuemin=\"0\"  \
                                              aria-valuemax=\"100\" style=\"width:" + 100 * percentage + "%\"> \
                                                  <span style=\"color:black\">" + data.trade_quantity + '/' + data.quantity + "</span>\
                                              </div> \
                                          </div>\
                                        </td> \
                                        <td id='" + data.id + "_open_close'>" + data.open_close + "</td> \
                                        <td id='" + data.id + "_accountOrder'>" + data.account + "</td> \
                                        <td id='" + data.id + "_buy_sell'>" + data.buy_sell + "</td> \
                                        <td id='" + data.id + "_argument_list'>" + data.argument_list + "</td> \
                                        <td id='" + data.id + "_id'>" + data.id + "</td> \
                                    </tr>");
    }
    $('#' + data.id + "_progress").find('span').css("color", "black");
    if (data.buy_sell == "SHORT_SELL") {
        $('#' + data.id + '_buy_sell').attr("class", "danger");
    } else if (data.buy_sell == "LONG_BUY") {
        $('#' + data.id + '_buy_sell').attr("class", "info");
    }
    changeStatusColor(data.id, data.status)
    //document.getElementById('debug').insertRow().insertCell().innerHTML = orderObj;
}
