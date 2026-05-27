## TODO:
 - [ ] Use ring buffer for audio playback to avoid lagging/choppiness
 - - Worker thread processes data with stretcher, uses read index (if there's a chunk of space available to write)
 - - maDataCallback reads next data from ring buffer, updates UI position. If there's not enough for one chunk, fill the rest with zeroes.
 - - Seeking: pause worker thread with flag, flush ring buffer and reset stretcher, update readIndex and update UI position, unpause worker thread
 - [ ] Improve click capture for seeking
 - [ ] Pause when song is finished
 - [ ] Speed dial in UI
 - [ ] Visualisation? Album Cover? 
 - [ ] Setup Windows, MacOS and Android builds