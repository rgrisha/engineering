-- bevielio SSID ir slaptažodis
dofile("credentials.lua")


function pin_cb()
	print("Paspaustas mygtukas siųsti dabar... ")
  send_measurements()
end

function measure_now_init()
  local button_pin = 8 
  gpio.mode(button_pin, gpio.INT, gpio.PULLUP)
  gpio.trig(button_pin, "up", pin_cb)
end

function startup()
    if file.open("init.lua") == nil then
        print("init.lua deleted or renamed")
    else
        print("Veikia")
        file.close("init.lua")
        measure_now_init()
    end
end

local is_connected = false

-- Prisijungimas prie Wifi
wifi_connect_event = function(T) 
  print("Prisijungta prie Wifi SSID"..T.SSID..)
  print("Laukiamas IP adresas...")
  if disconnect_ct ~= nil then disconnect_ct = nil end  
end

wifi_got_ip_event = function(T) 
  -- Note: Having an IP address does not mean there is internet access!
  -- Internet connectivity can be determined with net.dns.resolve().    
  print("Pavyko Wifi sujungimas. IP adresas: "..T.IP)
  print("Paleidimas po 3 sek...")
  is_connected = true
  tmr.create():alarm(3000, tmr.ALARM_SINGLE, startup)

  local apitimer = tmr.create()
  --apitimer:register(60000 * 5, tmr.ALARM_AUTO, send_measurements)
  apitimer:register(60000 * 1, tmr.ALARM_AUTO, send_measurements)
  apitimer:start()

end

wifi_disconnect_event = function(T)

  is_connected = false

  if T.reason == wifi.eventmon.reason.ASSOC_LEAVE then 
    --the station has disassociated from a previously connected AP
    return 
  end
  -- total_tries: how many times the station will attempt to connect to the AP. Should consider AP reboot duration.
  local total_tries = 75
  print("\nWiFi prisijungimas prie "..T.SSID.." nepavyko!")

  --There are many possible disconnect reasons, the following iterates through 
  --the list and returns the string corresponding to the disconnect reason.
  for key,val in pairs(wifi.eventmon.reason) do
    if val == T.reason then
      print("Atsijungimo priežastis: "..val.."("..key..")")
      break
    end
  end

  if disconnect_ct == nil then 
    disconnect_ct = 1 
  else
    disconnect_ct = disconnect_ct + 1 
  end
  if disconnect_ct < total_tries then 
    print("Pakartotinis bandymas prisijungti ("..(disconnect_ct+1).." iš "..total_tries..")")
  else
    wifi.sta.disconnect()
    print("Baigiami bandymai prisijungti prie WiFi!")
    disconnect_ct = nil  
  end
end

-- Register WiFi Station event callbacks
wifi.eventmon.register(wifi.eventmon.STA_CONNECTED, wifi_connect_event)
wifi.eventmon.register(wifi.eventmon.STA_GOT_IP, wifi_got_ip_event)
wifi.eventmon.register(wifi.eventmon.STA_DISCONNECTED, wifi_disconnect_event)

print("Jungiamasi prie WiFi, inicializacija...")
wifi.setmode(wifi.STATION)
wifi.sta.config({ssid=SSID, pwd=PASSWORD})

----------------------------

function read_humidity() 
  local pin = 5
  local status, temp, humi, temp_dec, humi_dec = dht.read(pin)
  if status == dht.OK then
    return temp, humi

  elseif status == dht.ERROR_CHECKSUM then
      print( "DHT Checksum error." )
      return nil, nil
  elseif status == dht.ERROR_TIMEOUT then
    print( "DHT timed out." )
    return nil, nil
  end
end

function read_pressure()
  local sda = 6
  local scl = 7
  i2c.setup(0, sda, scl, i2c.SLOW)

  bmp085.setup()
  t = bmp085.temperature()
  p = bmp085.pressure()

  return t, p

end


function send_measurements(t)

  if is_connected == false then
    return 
  end

  local json = "{"
  local sep = ""
  if is_connected then
    th, hh = read_humidity()
    if th ~= nil then
      sep = ","
      json = json..'"hum-t":"'..th..'","hum-h":"'..hh..'"'
    end
    pt, pp = read_pressure()
    if pt ~= nil then
      json = json..sep..'"pres-t":"'..(pt/10).."."..(pt%10)..'","pres-p":"'..(pp/100).."."..(pp%100)..'"'
    end
    json = json.."}"

    local url="http://45.125.66.139/api/measurement"

    print( "will send json:", json)

    http.post(url, 'Content-Type: application/json\r\n', json,
      function(code, data)
        if (code < 0) then
          print("HTTP request failed")
        else
          print("posted json, result: ", code, data)
        end
      end)
  end
end

