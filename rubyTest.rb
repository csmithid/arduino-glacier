require 'json'
require 'open-uri'

json = open('https://raw.githubusercontent.com/csmithid/arduino-glacier/master/queries.json').read
hash = JSON.parse(json)

puts hash['queries'][1]
