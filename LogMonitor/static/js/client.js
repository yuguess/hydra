var ws;

var batchFocus = "NOFOCUS";
var securityFocus = "NOFOCUS";
var UserList = new Array();
var AcctList = new Array();
var DateList = new Array();
var userFocus = "All";
var acctFocus = "All";
var dateFocus = "All";

var test;

function init() {

  // Connect to Web Socket
 // ws = new WebSocket("ws://192.168.0.92:9001/");
  //ws = new WebSocket("ws://192.168.0.92:9527/soc");
  // Set event handlers.
  ws.onopen = function() {

  };

  ws.onmessage = function(e) {
    // e.data contains received string.
    //document.getElementById('debug').insertRow().insertCell().innerHTML = e.data;
    updateWeb(e.data);
    actionBinding();


  };

  ws.onclose = function() {

  };

  ws.onerror = function(e) {
    console.log(e)
  };

  actionBinding();
}

function updateWeb(str)
{
  if (str[0] == "{")
  {
    dataObj = eval("("+str+")");
    switch (dataObj["returnType"])
    {
      case 1:
        //batch
        if ($.inArray(dataObj['account'], AcctList) == -1)
        {
          AcctList.push(dataObj['account']);
          $('#acct').html($('#acct').html() +"<button class='btn acct col-md-1'>"+ dataObj['account'] + "</button>");
        };
        if ($.inArray(dataObj['userID'], UserList) == -1)
        {
          UserList.push(dataObj['userID']);
          $('#user').html($('#user').html() +"<button class='btn user col-md-1'>"+ dataObj['userID'] + "</button>");
        };
        var datestring = dataObj['updateTime'].substring(0,8);
        if ($.inArray(datestring, DateList) == -1)
        {
          DateList.push(datestring);
          $('#date').html($('#date').html() +"<button class='btn date col-md-1'>"+ datestring + "</button>");
        };


        for (var i = AcctList.length - 1; i >= 0; i--) {

        };

        constructBatchFuture(dataObj);
        if (dataObj['progress'] < 0.01)
        {
        }
        break;
      case 2:
        //security
        constructSecurity(dataObj);
        break;
      case 3:
        //trade
        constructTradeFuture(dataObj);
        if(dataObj["m_Direction"] == "BUY")
        {
          $directionZh = "买入";
        }
        else
        {
          $directionZh = "卖出";
        }
        //$("#audio source").attr("src","http://tts.baidu.com/text2audio?idx=1&tex="+dataObj["m_ProdID"]+" "+$directionZh+" "+"成交  "+dataObj["m_Volume"]+"股&cuid=baidu speed demo&cod=2&lan=zh&ctp=1&pdt=1&spd=9&per=0&vol=5&pit=5")
	//$("#audio source").attr("src","http://tts.baidu.com/text2audio?idx=1&tex="+dataObj["m_ProdID"]+" "+$directionZh+"&cuid=baidu speed demo&cod=2&lan=zh&ctp=1&pdt=1&spd=9&per=0&vol=5&pit=5")
    $("#audio source").attr("src","http://tsn.baidu.com/text2audio?tex=" + dataObj["m_ProdID"] + "成交\&lan=zh&cuid=" + "94-DE-80-23-E5-A6" + "\&spd=8\&ctp=1&tok="+"24.e175ed83539ebe33d2eb67c61effe559.2592000.1475223151.282335-8572922");

        // $("#audio source").attr("src","sound/Trade_1Ring.mp3")

        $("#audio")[0].pause();
        $("#audio")[0].load();
        $("#audio")[0].play();

        break;
      case 4:
         $('.security[batchID='+dataObj["m_BatchID"]+'][security='+dataObj['Product']+'] .OrderStatus').removeAttr("style");
        $('.security[batchID='+dataObj["m_BatchID"]+'][security='+dataObj['Product']+'] .OrderStatus').html("Status: "+dataObj['m_OrderStatus']+" Price: "+dataObj['LimitPrice']);
        if (dataObj['m_OrderStatus'] == "Accepted")
        {
            $('.security[batchID='+dataObj["m_BatchID"]+'][security='+dataObj['Product']+'] .OrderStatus').attr("style","color:Green;");
        };
        if (dataObj['m_OrderStatus'] == "Rejected")
        {
            $('.security[batchID='+dataObj["m_BatchID"]+'][security='+dataObj['Product']+'] .OrderStatus').attr("style","color:Red;");
        };
        break;
      default:
        console.log(dataObj);
    }
  }
  else
  {
    console.log(str);
  };
}

function showAcctOrUser()
{
  if (dateFocus == "All")
  {
    dateFilter = "";
  }
  else
  {
    dateFilter = "[date='"+dateFocus+"']";
  }

  if (userFocus == "All")
  {
    userFilter = "";
  }
  else
  {
    userFilter = "[user='"+userFocus+"']";
  }

  if (acctFocus == "All")
  {
    acctFilter = "";
  }
  else
  {
    acctFilter = "[acct='"+acctFocus+"']";
  }

  $(".batchOda").hide();
  $(".security").hide();
  $(".trade").hide();
  $(".batchOda"+acctFilter+userFilter+dateFilter).show();
}

function showBatch (batchID) {
  // body...
  $(".security").hide();
  $(".trade").hide();
  $(".security[batchID="+batchID+"]").show();
}

function showSec (batchID, securityID) {
  $(".trade").hide();
  $(".trade[security='"+securityID+"']"+"[batchID='"+batchID+"']").show();
}


function actionBinding () {

  //hide
  showBatch(batchFocus);
  showSec(batchFocus, securityFocus);

    ///////////////Click actions

  $(".batchOda").unbind('click').click(function(event) {
    /* Act on the event */
    batchID = $(this).attr("batchID");
    showBatch(batchID);
    batchFocus = batchID;
  });

  $(".security").unbind('click').click(function(event) {
    /* Act on the event */
    batchID = $(this).attr("batchID");
    security = $(this).attr("security");
    showSec(batchID, security);
    securityFocus = security;
   });

  $(".acct").unbind('click').click(function(event) {
    /* Act on the event */
    acctFocus = $(this).html();
    $(".acct").removeClass('btn-success');
    $(this).addClass('btn-success');
    showAcctOrUser();
  });

  $(".user").unbind('click').click(function(event) {
    /* Act on the event */
    userFocus = $(this).html();
    $(".user").removeClass('btn-success');
    $(this).addClass('btn-success');
    showAcctOrUser();
  });

  $(".date").unbind('click').click(function(event) {
    /* Act on the event */
    dateFocus = $(this).html();
    $(".date").removeClass('btn-success');
    $(this).addClass('btn-success');
    showAcctOrUser();
  });




}

batchObj = '{"batchID":46197,"notional":0,"progress":98,"returnType":1,"shares":0,"slippage1":0,"slippage2":0,"updateTime":"20150821 08:46:18"}';
securityObj = '{"avgPrice":0,"batchID":46197,"direction":"BUY","fillRate":100,"filledQty":0,"quantity":2000,"refPxBegin":0,"refVWAP":0,"returnType":2,"slippage1":0,"slippage2":0,"ticker":"SH600000","totalNotional":0,"updateTime":"20150821 08:46:19","urgency":0}';
tradeObj = '{"avgPrice":0,"batchID":46197,"direction":"BUY","quantity":2000,"returnType":3,"ticker":"SH600000","updateTime":"20150821 08:46:19"}';

function killBatch(batchID){
	ws.send('[{"BatchID":"'+batchID+'", "ActionType": "KILL", "User": "Web", "Random": "XXXX"}]');
	console.log('[{"BatchID":"'+batchID+'", "ActionType": "KILL", "User": "Web", "Random": "XXXX"}]');
}


function constructBatchFuture(batchObj){
  webObj = $(".batchOda[batchID="+batchObj['batchID']+"]");
  datestring = batchObj['updateTime'].substring(0,8);

  if (webObj.length == 0)
  {
    $batchPrototype = $('<tr class="batchOda" batchID="" user="" acct="" date="">\
                  <td class="batchID"></td>\
                  <td>\
                    <div class="progress">\
                      <div class="progress-bar progress-bar-striped active" role="progressbar" aria-valuenow="40" aria-valuemin="0" aria-valuemax="100" style="width:80%;">\
                      </div>\
                      <span class="progress-text"></span>\
                    </div>\
                  </td>\
                  <td class="amount"></td>\
                  <td class="slippage1"></td>\
                  <td class="updatetime"></td>\
                  <td class="btype"></td>\
                  <td class="binfo"></td>\
                  <td class="kill"></td>\
                </tr>');
    $batchPrototype.attr("batchID", batchObj['batchID']);
    $batchPrototype.attr("date", datestring);
    $batchPrototype.attr("user", batchObj['userID']);
    $batchPrototype.attr("acct", batchObj['account']);
    $batchPrototype.find(".batchID").html(batchObj['batchID']);
    $progressbar = $batchPrototype.find("div.progress div");
    if (batchObj['progress'] - 100 > -1) {
      $progressbar.removeClass("active");
      $progressbar.removeClass("progress-bar-striped");
    };
    $progressbar.attr("aria-valuenow", batchObj['progress']);
    $progressbar.attr("style", 'width:' + batchObj['progress'] + '%');
    $batchPrototype.find(".progress-text").html(batchObj['progress'].toFixed(2) + '% ');
    $batchPrototype.find(".progress-text").attr("title","总金额："+batchObj['notional']);
    $batchPrototype.find(".amount").html(batchObj['notional'].toFixed(2));
    $batchPrototype.find(".slippage1").html((batchObj['slippage1']*10000).toFixed(2)+"BP");
    $batchPrototype.find(".updatetime").html(batchObj['updateTime']);
    $batchPrototype.find(".btype").html(batchObj['batchType']);
    $batchPrototype.find(".binfo").html(batchObj['batchInfo']);
    $batchPrototype.find(".kill").html("<div class='btn btn-info btn-block btn-xs' style='height:20px' onClick='$(this).attr(\"class\",\"btn btn-danger btn-block btn-xs\");killBatch("+batchObj["batchID"]+")'>    </div>");
    $("#batchTable tbody").prepend($batchPrototype);
          $("#audio source").attr("src","static/sound/BatchOrder_2Ring.mp3")

          $("#audio")[0].pause();
          $("#audio")[0].load();
          $("#audio")[0].play();

  }
  else
  {
    $batchPrototype = webObj;

    $batchPrototype.attr("batchID", batchObj['batchID']);
    $batchPrototype.attr("date", datestring);
    $batchPrototype.attr("user", batchObj['userID']);
    $batchPrototype.attr("acct", batchObj['account']);
    $batchPrototype.find(".batchID").html(batchObj['batchID']);
    $progressbar = $batchPrototype.find("div.progress div");
    if (batchObj['progress'] - 100 > -1) {
      $progressbar.removeClass("active");
      $progressbar.removeClass("progress-bar-striped");
    };
    $progressbar.attr("aria-valuenow", batchObj['progress']);
    $progressbar.attr("style", 'width:' + batchObj['progress'] + '%');
    $batchPrototype.find(".progress-text").html(batchObj['progress'].toFixed(2) + '% ');
    $batchPrototype.find(".progress-text").attr("title","总金额："+batchObj['notional']);
    $batchPrototype.find(".amount").html(batchObj['notional'].toFixed(2));
    $batchPrototype.find(".slippage1").html((batchObj['slippage1']*10000).toFixed(2)+"BP");
    $batchPrototype.find(".btype").html(batchObj['batchType']);
    $batchPrototype.find(".binfo").html(batchObj['batchInfo']);
    $batchPrototype.find(".updatetime").html(batchObj['updateTime']);

  }
}

function constructSecurity(securityObj){
  webObj = $(".security[batchID="+securityObj['batchID']+"][security="+securityObj['ticker']+"]");
  tradeObj = $(".tradeTable[batchID="+securityObj['batchID']+"][security="+securityObj['ticker']+"]");

  if (webObj.length == 0)
  {
    $securityPrototype = $('<tr class="security" batchID = "" security="">\
                  <td class="batchID"></td>\
                  <td class="securityID"></td>\
                  <td class="direction"></td>\
                  <td>\
                    <div class="progress">\
                      <div class="progress-bar progress-bar-striped active" role="progressbar" aria-valuenow="40" aria-valuemin="0" aria-valuemax="100" style="width:50%;">\
                      </div>\
                      <span class="progress-text"></span>\
                    </div>\
                  </td>\
                  <td class="urgency"></td>\
                  <td class="OriginRef"></td>\
                  <td class="avgPrice"></td>\
                  <td class="slippage1"></td>\
                  <td class="updatetime"></td>\
                  <td class="OrderStatus">Status: Init Price: N/A</td>\
                </tr>');
    $securityTrade = $('<tr class="tradeTable trade" batchID="" security="" class="secTrade"><td colspan="8" style="display:none;"><table><thead><tr><th>BatchID</th><th>Ticker</th><th>Direction</th><th>Qty</th><th>avgPrice</th><th>TradeTime</th></tr></thead><tbody></tbody></table></td></tr>')
    $securityPrototype.attr('batchID', securityObj['batchID']);
    $securityPrototype.attr('security', securityObj['ticker']);
    $securityTrade.attr('batchID', securityObj['batchID']);
    $securityTrade.attr('security', securityObj['ticker']);
    $securityPrototype.find('.batchID').html(securityObj['batchID']);
    $securityPrototype.find('.securityID').html(securityObj['ticker']);
    $securityPrototype.find('.direction').html(securityObj['direction']);
    $securityPrototype.find('.urgency').html(securityObj['urgency']);
    $securityPrototype.find('.OriginRef').html(securityObj['refPxBegin'].toFixed(4));
    $securityPrototype.find('.avgPrice').html(securityObj['avgPrice'].toFixed(4));
    $securityPrototype.find('.slippage1').html((securityObj['slippage1']*10000).toFixed(2)+"BP");
    $securityPrototype.find('.updatetime').html(securityObj['updateTime']);
    if (securityObj['fillRate']*100 - 100 > -1)
    {
      $securityPrototype.find(".progress-bar").removeClass("active");
      $securityPrototype.find(".progress-bar").removeClass("progress-bar-striped");
    };
    $securityPrototype.find(".progress-bar").attr("aria-valuenow", securityObj['fillRate']*100);
    $securityPrototype.find(".progress-bar").attr("style", 'width:' + securityObj['fillRate']*100 + '%');
    $securityPrototype.find(".progress-text").html(securityObj['fillRate'].toFixed(2)*100 + '%');
    $securityPrototype.find(".progress-text").attr("title", securityObj['filledQty'] + " / " + securityObj["quantity"]);

    $("#secTable>tbody").append($securityPrototype);
    $("#secTable>tbody").append($securityTrade);
  }
  else
  {
    $securityPrototype = webObj;
    $securityTrade = tradeObj;

    $securityPrototype.attr('batchID', securityObj['batchID']);
    $securityPrototype.attr('security', securityObj['ticker']);
    $securityTrade.attr('batchID', securityObj['batchID']);
    $securityTrade.attr('security', securityObj['ticker']);
    $securityPrototype.find('.batchID').html(securityObj['batchID']);
    $securityPrototype.find('.securityID').html(securityObj['ticker']);
    $securityPrototype.find('.direction').html(securityObj['direction']);
    $securityPrototype.find('.urgency').html(securityObj['urgency']);
    $securityPrototype.find('.OriginRef').html(securityObj['refPxBegin'].toFixed(4));
    $securityPrototype.find('.avgPrice').html(securityObj['avgPrice'].toFixed(4));
    $securityPrototype.find('.slippage1').html((securityObj['slippage1']*10000).toFixed(2)+"BP");
    $securityPrototype.find('.updatetime').html(securityObj['updateTime']);
    if (securityObj['fillRate']*100 - 100 > -1)
    {
      $securityPrototype.find(".progress-bar").removeClass("active");
      $securityPrototype.find(".progress-bar").removeClass("progress-bar-striped");
    };
    $securityPrototype.find(".progress-bar").attr("aria-valuenow", securityObj['fillRate']*100);
    $securityPrototype.find(".progress-bar").attr("style", 'width:' + securityObj['fillRate']*100 + '%');
    $securityPrototype.find(".progress-text").html(securityObj['fillRate'].toFixed(2)*100 + '%');
    $securityPrototype.find(".progress-text").attr("title", securityObj['filledQty'] + " / " + securityObj["quantity"]);
  }
}

function constructTradeFuture(tradeObj){

  $tradePrototype = $('<tr class="trade" batchID="" security="">\
                <td class="batchID">10002</td>\
                <td class="securityID">000001.SH</td>\
                <td class="direction">BUY</td>\
                <td class="quantity">\
                  500\
                </td>\
                <td class="tradePrice">13.23</td>\
                <td class="tradetime">08:40:56</td>\
              </tr>');
  $tradePrototype.attr("batchID", tradeObj["m_BatchID"]);
  $tradePrototype.attr("security", tradeObj["m_ProdID"]);
  $tradePrototype.find(".batchID").html(tradeObj["m_BatchID"]);
  $tradePrototype.find(".securityID").html(tradeObj["m_ProdID"]);
  $tradePrototype.find(".direction").html(tradeObj["m_Direction"]);
  $tradePrototype.find(".quantity").html(tradeObj["m_Volume"]);
  $tradePrototype.find(".tradePrice").html(tradeObj["m_Price"].toFixed(4));
  $tradePrototype.find(".tradetime").html(tradeObj["m_TradeTime"]);

  $('.tradeTable[batchID='+tradeObj["m_BatchID"]+'][security='+tradeObj["m_ProdID"]+'] table>tbody').append($tradePrototype);
  $('.tradeTable[batchID='+tradeObj["m_BatchID"]+'][security='+tradeObj["m_ProdID"]+']>td').attr("style","display:true");
  // $('#tradeTable tbody').append($tradePrototype);

}


$(document).ready(init());
