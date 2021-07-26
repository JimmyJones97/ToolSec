-- 定位Names

local target_string = 'Default__ActorComponent'
local bytes = gg.bytes(target_string, 'UTF-8')
local hex_text = 'h'
for k,v in pairs(bytes) do
	--print(k, v)
	hex_text = hex_text .. string.format('%x', v) .. ' '
end
print('Search:', hex_text)
--print('UTF-8', bytes)
gg.searchNumber(hex_text)

local resultsCount = gg.getResultsCount()
local results = gg.getResults(10)
print('results count:', resultsCount)
print('results 1:', results[1])

local r1 = results[1]

local FNameEntry_addr = r1.address - 8
local t = {}
t[1] = {}
t[1].address = FNameEntry_addr
t[1].flags = 4 -- 使用gg.TYPE_DWORD会报nil
t[2] = {}
t[2].address = FNameEntry_addr + 4
t[2].flags = 4
t = gg.getValues(t)
print(t)
-- iglite的FNameEntry结构没有变动(pubgmhd有变动)
local FNameEntry = {}
FNameEntry['Index'] = t[1].value
FNameEntry['HashNext'] = t[2].value
FNameEntry['AnsiName'] = target_string
print('FNameEntry:', FNameEntry)


---------------------------------------
gg.clearResults()


local WithinIndex = (FNameEntry['Index'] >> 1) % 16384
local ChunkIndex = (FNameEntry['Index'] >> 1)/ 16384
print('WithinIndex:', WithinIndex, ', ChunkIndex:', ChunkIndex)

-- void *addr = &FNameEntry, 反搜addr，找到Chunk中的一个FNameEntry*

local FNameEntry_addr_for_search = string.format("%xh", FNameEntry_addr)
print('FNameEntry_addr_for_search:', FNameEntry_addr_for_search)
gg.copyText(FNameEntry_addr_for_search)
gg.searchNumber(FNameEntry_addr_for_search, gg.TYPE_DWORD)
local resultsCount = gg.getResultsCount()
local results = gg.getResults(10)
print('resultsCount:', resultsCount)
print('results 1:', results)

local FNameEntry_pointer_addr = results[1].address
local Chunk_addr = FNameEntry_pointer_addr - 4 * WithinIndex
local Chunk_tail_addr = Chunk_addr + 4 * 16384
print(string.format('FNameEntry_pointer_addr:0x%x, Chunk_addr:0x%x, Chunk_tail_addr:0x%x', FNameEntry_pointer_addr, Chunk_addr, Chunk_tail_addr))
local Chunk_tail_addr_for_search = string.format("%xh", Chunk_tail_addr)
--gg.copyText(Chunk_tail_addr_for_search)
-- -----------------------------------------
gg.clearResults()


local Chunk_addr_for_search = string.format("%xh", Chunk_addr)
print('Chunk_addr_for_search:', Chunk_addr_for_search)
--gg.copyText(Chunk_addr_for_search)
gg.searchNumber(Chunk_addr_for_search, gg.TYPE_DWORD)
local resultsCount = gg.getResultsCount()
local results = gg.getResults(10)
print('resultsCount:', resultsCount)
print('results 1:', results)

local Chunk_pointer_addr = results[1].address
local Chunks_addr = Chunk_pointer_addr - 4 * ChunkIndex
print(string.format('Chunk_pointer_addr:0x%x, Chunks_addr:0x%x', Chunk_pointer_addr, Chunks_addr))


-- ----------------------------------------
gg.clearResults()

-- iglite中，反搜Chunks_addr，有55个结果，找不到一个基于libUE4.so偏移中的值
-- 猜测：1. 做了加密；2. 增加了若干层
local Chunks_addr_for_search = string.format("%xh", Chunks_addr)
print('Chunks_addr_for_search:', Chunks_addr_for_search)
gg.copyText(Chunks_addr_for_search)
gg.searchNumber(Chunks_addr_for_search, gg.TYPE_DWORD)
local resultsCount = gg.getResultsCount()
local results = gg.getResults(100)
print('resultsCount:', resultsCount)
print('results 1:', results)

-- 73C6094
-- 70E3A5C
