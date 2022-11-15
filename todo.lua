require 'cairo'

-- the path to your "events" file, which stores the list data
EVENT_PATH = "/home/reimu/.config/conky/lua/events"

function m_to_date(m)
	local mins_until = ((m - CURRENT_DATE)//60)
	if mins_until < 0 then
		return "invalid"
	end
	local d_mins = (mins_until % 60)
	local h = (mins_until // 60)
	local d_hrs = (h % 24)
	local d_days = (h // 24)
	if tonumber(d_days) < 10 then
		d_days = "0"..d_days
	end
	if tonumber(d_hrs) < 10 then
		d_hrs = "0"..d_hrs
	end
	if tonumber(d_mins) < 10 then
		d_mins = "0"..d_mins
	end
	return d_days..":"..d_hrs..":"..d_mins
end

function parse_events()
	local file = io.open(EVENT_PATH,"rb")
	if not file then return nil end
	local events = {}
	for line in io.lines(EVENT_PATH) do
		local event = {}
		for str in string.gmatch(line, "([^".."|".."]+)") do
			table.insert(event,str)
		end
		table.insert(events,event)
	end
	return events
end

function out(cr,tx,x,y)
  -- print text.
	f="Montserrat Light"
	fs = 15
	cairo_select_font_face(cr,f,CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_NORMAL)
	cairo_set_font_size(cr,fs)
	cairo_set_source_rgba(cr,133,133,133,0.6)
	cairo_move_to(cr,x,y)
	cairo_show_text(cr,tx)
	cairo_stroke(cr)
end

function checkbox(cr,x,y,checked)
  -- create checkbox
	line_width = 1
	box_size = 10
	cairo_set_line_width(cr,line_width)
	cairo_set_source_rgba(cr,102,102,102,1)
	cairo_rectangle(cr,x,y,box_size,box_size)
	if checked=="1" then 
		cairo_fill_preserve(cr)
	end
	cairo_stroke(cr)
end

function list_item(cr,pos,text,checked,time)
	y_pos = (50 + 25*(pos-1))
	checkbox(cr,15,y_pos,checked)
	out(cr,text,35,y_pos+10)
	out(cr,time,230,y_pos+10)
end

function conky_main()
	if conky_window == nil then return end
	local cs = cairo_xlib_surface_create(conky_window.display, conky_window.drawable, conky_window.visual, conky_window.width, conky_window.height)
	cr = cairo_create(cs)
	--------------------- boilerplate
	CURRENT_DATE = os.time(os.date('*t'))

	BOXES = parse_events()
	for i,box in ipairs(BOXES) do
		list_item(cr,i,box[1],box[2],m_to_date(box[3]))
	end
	--------------------- boilerplate
	cairo_destroy(cr)
	cairo_surface_destroy(cs)
	cr=nil

end -- end main function
